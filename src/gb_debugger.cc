#include <streambuf>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include <chrono>

#include "gb_debugger.h"
#include "gb_io_defs.h"
#include "gb_logger.h"

class ncurses_buf : public std::streambuf {
public:
    WINDOW* m_win;

    ncurses_buf() {}
    ~ncurses_buf() {}

    void setWindow(WINDOW* new_win) {
        m_win = new_win;
    }
    virtual int overflow(int c);
};

int ncurses_buf::overflow(int c) {
    wprintw(m_win, "%c", c);
    return c;
}

class ncurses_stream : public std::ostream {
public:
    ncurses_buf m_tbuf;
    std::ostream &m_src;
    std::streambuf* const m_srcbuf;

    ncurses_stream(std::ostream &o, WINDOW* win)
        : std::ostream(&m_tbuf), m_src(o), m_srcbuf(o.rdbuf()) {
        o.rdbuf(rdbuf());
        m_tbuf.setWindow(win);
    }

    ~ncurses_stream();
};

ncurses_stream::~ncurses_stream() {
    m_src.rdbuf(m_srcbuf);
}

#define GB_DEBUGGER_WAIT_MS            (50)
#define GB_DEBUGGER_NWIN_MAX_LINES     (10000)
#define GB_DEBUGGER_NWIN_MAX_LINES_STR "10000"

#define COMMAND_DOC_LIST \
{\
    {"h", "Print out this command list"},\
    {"n", "Step through one instruction"},\
    {"m", "Modify or view a single 8-bit or 16-bit register. Syntax: hl | hl=0xff00"},\
    {"r", "Dump all registers and flags"},\
    {"x", "Examine or modify a single 8-bit memory location. Syntax: 0xff80 | 0xff80=0xff"},\
    {"b", "Set, clear, delete or list breakpoints. Sytanx: set 0xff80 | del 0xff80 | clear | list"},\
    {"c", "Continue or halt execution of CPU with instruction tracing enabled"},\
    {"C", "Continue or halt execution of CPU with instruction tracing disabled. Halting will re-enable tracing"},\
    {"s", "Save the last " GB_DEBUGGER_NWIN_MAX_LINES_STR " of the debugger trace to a file"},\
    {"u", "Scroll up half a page"},\
    {"d", "Scroll down half a page"},\
    {"G", "Scroll to beginning"},\
    {"g", "Scroll to end"},\
    {"Up", "Scroll up one line"},\
    {"Down", "Scroll down one line"},\
    {"q", "Quit"}\
}

#define KEY_MAP_INIT \
{\
    {'h', std::bind(&gb_debugger::_debugger_help, this)},\
    {'n', std::bind(&gb_debugger::_debugger_step_once, this)},\
    {'m', std::bind(&gb_debugger::_debugger_modify_register, this)},\
    {'r', std::bind(&gb_debugger::_debugger_dump_registers, this)},\
    {'x', std::bind(&gb_debugger::_debugger_access_memory, this)},\
    {'b', std::bind(&gb_debugger::_debugger_breakpoints, this)},\
    {'c', std::bind(&gb_debugger::_debugger_toggle_continue, this)},\
    {'C', std::bind(&gb_debugger::_debugger_toggle_continue_and_tracing, this)},\
    {'s', std::bind(&gb_debugger::_debugger_save_trace, this)},\
    {'u', std::bind(&gb_debugger::_debugger_scroll_up_half_pg, this)},\
    {'d', std::bind(&gb_debugger::_debugger_scroll_dn_half_pg, this)},\
    {'G', std::bind(&gb_debugger::_debugger_scroll_to_start, this)},\
    {'g', std::bind(&gb_debugger::_debugger_scroll_to_end, this)},\
    {KEY_UP, std::bind(&gb_debugger::_debugger_scroll_up_one_line, this)},\
    {KEY_DOWN, std::bind(&gb_debugger::_debugger_scroll_dn_one_line, this)}\
}

gb_debugger::gb_debugger(gb_emulator& emulator)
    : m_emulator(emulator), m_key_map(KEY_MAP_INIT), m_command_doc(COMMAND_DOC_LIST), m_nwin_pos(0),
      m_nwin_max_lines(GB_DEBUGGER_NWIN_MAX_LINES), m_nwin_lines(0), m_nwin_cols(0), m_frame_cycles(0), m_continue(false)
{
    // Initialize the ncurses library, disable line-buffering and disable character echoing
    // Enable blocking on getch()
    initscr();
    noecho();
    cbreak();
    curs_set(false);

    m_nwin_lines = getmaxy(stdscr)-1;
    m_nwin_cols = getmaxx(stdscr);
    m_nwin = newpad(m_nwin_max_lines, m_nwin_cols);
    nodelay(m_nwin, true);
    keypad(m_nwin, true);
    scrollok(m_nwin, true);
    idlok(m_nwin, true);
    prefresh(m_nwin, 0, 0, 0, 0, m_nwin_lines, m_nwin_cols);

    // Save the cout stream and replace it with a custom one that works with ncurses
    // https://stackoverflow.com/questions/20126649/ncurses-and-ostream-class-method
    m_nstream = std::make_unique<ncurses_stream>(std::cout, m_nwin);
}

gb_debugger::~gb_debugger() {
    // Close the ncurses library
    if (m_nwin != nullptr) {
        delwin(m_nwin);
        endwin();
    }
}

void gb_debugger::go() {
    for(int c = wgetch(m_nwin); c != 'q' && m_emulator.m_renderer.is_open(); c = wgetch(m_nwin)) {
        if (m_continue) {
            try {
                m_frame_cycles += m_emulator.step(1000);
            } catch (const gb_breakpoint_exception& bp) {
                // Update cycle count from instruction prior to breakpoint
                m_frame_cycles += bp.get_last_cycle_count();
                m_continue = false;
                gb_logger::instance().enable_tracing(true);
                GB_LOGGER(GB_LOG_TRACE) << bp.what() << std::endl;
            }
        }

        try {
            key_handler_t key_handler = m_key_map.at(c);
            key_handler();
        } catch (const std::out_of_range& oor) {}

        prefresh(m_nwin, m_nwin_pos, 0, 0, 0, m_nwin_lines-1, m_nwin_cols);

        if (m_frame_cycles >= 70224) {
            m_frame_cycles = 0;
            m_emulator.m_renderer.update(((m_emulator.m_memory_map.read_byte(GB_LCDC_ADDR) & 0x80) != 0));
        }

        if (!m_continue) std::this_thread::sleep_for(std::chrono::milliseconds(GB_DEBUGGER_WAIT_MS));
    }
}

void gb_debugger::update_pos() {
    m_nwin_pos = std::max(getcury(m_nwin)-m_nwin_lines, m_nwin_pos);
}

void gb_debugger::clear_line(int line) {
    wmove(m_nwin, line, 0);
    wclrtoeol(m_nwin);
    prefresh(m_nwin, m_nwin_pos, 0, 0, 0, m_nwin_lines, m_nwin_cols);
}

void gb_debugger::restore_window(int line) {
    noecho();
    cbreak();
    nodelay(m_nwin, true);
    wmove(m_nwin, line, 0);
}

void gb_debugger::print_line(const std::string& str, int line) {
    clear_line(line);
    wmove(m_nwin, line, 0);
    GB_LOGGER(GB_LOG_TRACE) << str;
    prefresh(m_nwin, std::max(getcury(m_nwin)-m_nwin_lines, m_nwin_pos), 0, 0, 0, m_nwin_lines, m_nwin_cols);
}

void gb_debugger::wait_newline(int line) {
    print_line("Press ENTER to continue...", line);
    for (int c = wgetch(m_nwin); c != '\n' && c != '\r'; c = wgetch(m_nwin)) std::this_thread::sleep_for(std::chrono::milliseconds(GB_DEBUGGER_WAIT_MS));
    clear_line(line);
}

void gb_debugger::handle_exception(const std::string& str, const std::exception& e, int from_line, int to_line) {
    print_line(str + " -- " + e.what(), from_line);
    wait_newline(from_line+1);
    clear_line(from_line);
    restore_window(to_line);
}


std::string gb_debugger::get_string(int line, int col) {
    std::string input;
    nodelay(m_nwin, false);
    wmove(m_nwin, line, col);
    for (int c = wgetch(m_nwin); c != '\n' && c != '\r'; c = wgetch(m_nwin)) {
        if (c == KEY_BACKSPACE || c == '\b' || c == 127) {
            input.pop_back();
            wdelch(m_nwin);
            wmove(m_nwin, line, getcurx(m_nwin)-1);
        } else {
            input.push_back(static_cast<char>(c));
            GB_LOGGER(GB_LOG_TRACE) << input.back();
        }
        prefresh(m_nwin, m_nwin_pos, 0, 0, 0, m_nwin_lines, m_nwin_cols);
    }
    nodelay(m_nwin, true);

    return input;
}

void gb_debugger::_debugger_help() {
    // Get current Y and end-of-screen y
    int cur_y = getcury(m_nwin);
    int eos_y = std::max(m_nwin_lines, cur_y);

    // Go through command documentation and print it starting at eos_y
    for (const std::array<std::string, 2>& command : m_command_doc) {
        long i = &command - &m_command_doc[0];
        print_line(command[0] + "\t- " + command[1], eos_y + static_cast<int>(i));
    }

    // Wait for enter
    wait_newline(eos_y + static_cast<int>(m_command_doc.size()));

    // Clear lines & restore window
    for (size_t i = 0; i < m_command_doc.size(); i++) clear_line(eos_y+static_cast<int>(i));
    restore_window(cur_y);
    update_pos();
}

void gb_debugger::_debugger_step_once() {
    try {
        m_frame_cycles += m_emulator.step(4);
    } catch (const gb_breakpoint_exception& bp) {
        // Update cycle count from instruction prior to breakpoint
        m_frame_cycles += bp.get_last_cycle_count();
        m_continue = false;
        gb_logger::instance().enable_tracing(true);
        GB_LOGGER(GB_LOG_TRACE) << bp.what() << std::endl;
    }
    update_pos();
}

void gb_debugger::_debugger_dump_registers() {
    m_emulator.m_cpu.dump_registers();
    update_pos();
}

void gb_debugger::_debugger_modify_register() {
    // Get current Y and end-of-screen y
    int cur_y = getcury(m_nwin);
    int eos_y = std::max(m_nwin_lines, cur_y);

    // Move to end of screen, clear it and print prompt
    print_line("Register: ", eos_y);

    // Wait for input
    std::string input = get_string(eos_y, getcurx(m_nwin));

    // Search for up to two substrings, the first is the register name and the second is the value to write to it
    size_t pos = input.find_first_of('=', 0);
    std::string reg;
    uint16_t data = 0;
    bool write = (pos != std::string::npos) ? true : false;

    try {
        reg = input.substr(0, pos);
        reg.erase(std::remove_if(reg.begin(), reg.end(), [] (char c) { return std::isspace(c); }), reg.end());
        std::transform(reg.begin(), reg.end(), reg.begin(), ::tolower);
    } catch (const std::exception& e) {
        handle_exception("gb_debugger::_debugger_modify_register()", e, eos_y, cur_y);
        return;
    }

    if (write) {
        try {
            data = static_cast<uint16_t>(std::stoul(input.substr(pos+1), nullptr, 0));
        } catch (const std::exception& e) {
            handle_exception("gb_debugger::_debugger_modify_register()", e, eos_y, cur_y);
            return;
        }
    }

    // Clear line & restore window
    clear_line(eos_y);
    restore_window(cur_y);

    auto do_reg = [data, write] (std::function<uint16_t()> get_reg, std::function<void()> set_reg) -> uint16_t {
        uint16_t d = data;
        if (write) set_reg();
        else d = get_reg();
        return d;
    };

    if (reg == "a") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_a, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_a, &m_emulator.m_cpu, 0, data));
    } else if (reg == "f") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_f, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_f, &m_emulator.m_cpu, 0, data));
    } else if (reg == "b") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_b, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_b, &m_emulator.m_cpu, 0, data));
    } else if (reg == "c") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_c, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_c, &m_emulator.m_cpu, 0, data));
    } else if (reg == "d") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_d, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_d, &m_emulator.m_cpu, 0, data));
    } else if (reg == "e") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_e, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_e, &m_emulator.m_cpu, 0, data));
    } else if (reg == "h") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_h, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_h, &m_emulator.m_cpu, 0, data));
    } else if (reg == "l") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_l, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_l, &m_emulator.m_cpu, 0, data));
    } else if (reg == "sp") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_sp, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_sp, &m_emulator.m_cpu, 0, data));
    } else if (reg == "pc") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_pc, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_pc, &m_emulator.m_cpu, 0, data));
    } else if (reg == "af") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_af, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_af, &m_emulator.m_cpu, 0, data));
    } else if (reg == "bc") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_bc, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_bc, &m_emulator.m_cpu, 0, data));
    } else if (reg == "de") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_de, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_de, &m_emulator.m_cpu, 0, data));
    } else if (reg == "hl") {
        data = do_reg(std::bind(&gb_cpu::_operand_get_register_hl, &m_emulator.m_cpu), std::bind(&gb_cpu::_operand_set_register_hl, &m_emulator.m_cpu, 0, data));
    } else {
        handle_exception("gb_debugger::_debugger_modify_register()", std::runtime_error("Unknown register"), eos_y, cur_y);
        return;
    }

    // Read or write data to a register
    if (write) GB_LOGGER(GB_LOG_TRACE) << "write: ";
    else GB_LOGGER(GB_LOG_TRACE) << "read: ";

    GB_LOGGER(GB_LOG_TRACE) << reg << " = " << "0x" << std::setfill('0') << std::setw(4) << std::hex << data << std::endl;
    update_pos();
}

void gb_debugger::_debugger_access_memory() {
    // Get current Y and end-of-screen y
    int cur_y = getcury(m_nwin);
    int eos_y = std::max(m_nwin_lines, cur_y);

    // Move to end of screen, clear it and print prompt
    print_line("Address: ", eos_y);

    // Wait for address input
    std::string input = get_string(eos_y, getcurx(m_nwin));

    // Search for up to two substrings, the first is the address and the second is the
    // optional data to write to the address separated by a '='
    size_t pos = input.find_first_of('=', 0);
    unsigned long addr = 0;
    unsigned long data = 0;
    bool write = (pos != std::string::npos) ? true : false;

    try {
        addr = std::stoul(input.substr(0, pos), nullptr, 0);
    } catch (const std::exception& e) {
        handle_exception("gb_debugger::_debugger_access_memory()", e, eos_y, cur_y);
        return;
    }

    if (write) {
        try {
            data = std::stoul(input.substr(pos+1), nullptr, 0);
        } catch (const std::exception& e) {
            handle_exception("gb_debugger::_debugger_access_memory()", e, eos_y, cur_y);
            return;
        }
    }

    // Clear line & restore window
    clear_line(eos_y);
    restore_window(cur_y);

    // Read or write data to memory
    if (write) {
        m_emulator.m_memory_map.write_byte(static_cast<uint16_t>(addr), static_cast<uint8_t>(data));
        GB_LOGGER(GB_LOG_TRACE) << "write: ";
    } else {
        data = m_emulator.m_memory_map.read_byte(static_cast<uint16_t>(addr));
        GB_LOGGER(GB_LOG_TRACE) << "read: ";
    }

    GB_LOGGER(GB_LOG_TRACE) << "0x" << std::setfill('0') << std::setw(4) << std::hex << addr << " = " << "0x" << std::setfill('0') << std::setw(2) << std::hex << data << std::endl;
    update_pos();
}

void gb_debugger::_debugger_breakpoints() {
    // Get current Y and end-of-screen y
    int cur_y = getcury(m_nwin);
    int eos_y = std::max(m_nwin_lines, cur_y);

    // Move to end of screen, clear it and print prompt
    print_line("Breakpoints: ", eos_y);

    // Wait for command input
    std::string input = get_string(eos_y, getcurx(m_nwin));

    // Tokenize input on whitespace
    std::istringstream iss (input);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));
    if (tokens.size() >= 1) std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

    unsigned int data = 0;
    auto _try_strtoul = [this, &data, tokens, eos_y, cur_y] () -> bool {
        if (tokens.size() <= 1) return false;
        try {
            data = static_cast<unsigned int>(std::stoul(tokens[1], nullptr, 0));
        } catch (const std::exception& e) {
            handle_exception("gb_debugger::_debugger_breakpoints()", e, eos_y, cur_y);
            return false;
        }
        return true;
    };

    auto _print_breakpoints = [this, eos_y] () -> void {
        std::ostringstream sstr;
        sstr << "Active breakpoints: ";
        for (auto bp : m_emulator.m_cpu.m_bp.m_breakpoints) {
            sstr << "0x" << std::hex << std::setfill('0') << std::setw(4) << bp << ", ";
        }
        print_line(sstr.str(), eos_y);
        wait_newline(eos_y+1);
    };

    if (tokens.size() == 0) {
    } else if (tokens[0] == "set" && _try_strtoul()) {
        m_emulator.m_cpu.m_bp.add(data);
        m_emulator.m_cpu.m_bp_enabled = true;
    } else if (tokens[0] == "del" && _try_strtoul()) {
        m_emulator.m_cpu.m_bp.remove(data);
        m_emulator.m_cpu.m_bp_enabled = (m_emulator.m_cpu.m_bp.m_breakpoints.size() != 0);
    } else if (tokens[0] == "clear") {
        m_emulator.m_cpu.m_bp.clear();
        m_emulator.m_cpu.m_bp_enabled = false;
    } else if (tokens[0] == "list") {
        _print_breakpoints();
    } else {
        handle_exception("gb_debugger::_debugger_breakpoints()", std::runtime_error("Unknown command: " + tokens[0]), eos_y, cur_y);
    }

    // Clear line & restore window
    clear_line(eos_y);
    restore_window(cur_y);
    update_pos();
}

void gb_debugger::_debugger_save_trace() {
    // Get current Y and end-of-screen y
    int cur_y = getcury(m_nwin);
    int eos_y = std::max(m_nwin_lines, cur_y);

    // Move to end of screen, clear it and print prompt
    print_line("Save file: ", eos_y);

    // Wait for command input and attempt to open file
    std::string input = get_string(eos_y, getcurx(m_nwin));

    std::ofstream file (input);

    if (!file) {
        handle_exception("gb_debugger::_debugger_save_trace()", std::runtime_error("Can't open file: " + input), eos_y, cur_y);
        clear_line(eos_y);
        restore_window(cur_y);
        return;
    }

    // Go through each line in the window and save it to the file
    char buf[256];
    for (int i = 0; i < cur_y; i++) {
        int c_cnt = mvwinnstr(m_nwin, i, 0, buf, 255);
        file << buf << ((buf[c_cnt-1] == '\n' ? "" : "\n"));
    }

    print_line("Saved to file: " + input, eos_y);
    wait_newline(eos_y+1);

    // Clear line & restore window
    clear_line(eos_y);
    restore_window(cur_y);
    update_pos();
}

void gb_debugger::_debugger_scroll_up_half_pg() {
    // Half page up
    m_nwin_pos = std::max(0, m_nwin_pos-(m_nwin_lines/2));
}

void gb_debugger::_debugger_scroll_dn_half_pg() {
    // Half page down
    m_nwin_pos = std::min(m_nwin_pos+(m_nwin_lines/2), std::max(0, getcury(m_nwin)-m_nwin_lines));
}

void gb_debugger::_debugger_scroll_to_start() {
    // To bottom of buffer
    m_nwin_pos = std::max(m_nwin_pos, getcury(m_nwin)-m_nwin_lines);
}

void gb_debugger::_debugger_scroll_to_end() {
    // To top of buffer
    m_nwin_pos = 0;
}

void gb_debugger::_debugger_scroll_up_one_line() {
    // 1 line up
    m_nwin_pos = std::max(0, m_nwin_pos-1);
}

void gb_debugger::_debugger_scroll_dn_one_line() {
    // 1 line down
    m_nwin_pos = std::min(m_nwin_pos+1, std::max(0, getcury(m_nwin)-m_nwin_lines));
}

void gb_debugger::_debugger_toggle_continue() {
    m_continue = m_continue ? false : true;
    gb_logger::instance().enable_tracing(true);
}

void gb_debugger::_debugger_toggle_continue_and_tracing() {
    _debugger_toggle_continue();
    gb_logger::instance().enable_tracing(!m_continue);
}
