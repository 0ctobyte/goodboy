#include <streambuf>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include <chrono>

#include <ncurses.h>

#include "gb_debugger.h"
#include "gb_io_defs.h"
#include "gb_logger.h"

#define COMMAND_DOC_LIST \
{\
    {"h", "Print out this command list"},\
    {"n", "Step through one instruction"},\
    {"m", "Modify or view a single 8-bit or 16-bit register. Syntax: hl | hl=0xff00"},\
    {"r", "Dump all registers and flags"},\
    {"x", "Examine or modify a single 8-bit memory location. Syntax: 0xff80 | 0xff80=0xff"},\
    {"b", "Set, clear, delete or list breakpoints. Sytanx: set 0xff80 | del 0xff80 | clear | list"},\
    {"w", "Set, clear, delete or list watchpoints. Sytanx: set 0xff80 | del 0xff80 | clear | list"},\
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
    {'w', std::bind(&gb_debugger::_debugger_watchpoints, this)},\
    {'c', std::bind(&gb_debugger::_debugger_toggle_continue, this)},\
    {'C', std::bind(&gb_debugger::_debugger_toggle_continue_and_tracing, this)},\
    {'s', std::bind(&gb_debugger::_debugger_save_trace, this)},\
    {'u', std::bind(&gb_debugger::_debugger_scroll_up_half_pg, this)},\
    {'d', std::bind(&gb_debugger::_debugger_scroll_dn_half_pg, this)},\
    {'g', std::bind(&gb_debugger::_debugger_scroll_to_start, this)},\
    {'G', std::bind(&gb_debugger::_debugger_scroll_to_end, this)},\
    {KEY_UP, std::bind(&gb_debugger::_debugger_scroll_up_one_line, this)},\
    {KEY_DOWN, std::bind(&gb_debugger::_debugger_scroll_dn_one_line, this)}\
}

#define GB_DEBUGGER_WAIT_MS            (50)
#define GB_DEBUGGER_NWIN_MAX_LINES     (10000)
#define GB_DEBUGGER_NWIN_MAX_LINES_STR "10000"

class ncurses_buf : public std::streambuf {
public:
    WINDOW* m_win;

    ncurses_buf() {}
    ~ncurses_buf() {}

    void set_window(WINDOW* new_win) {
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
        m_tbuf.set_window(win);
    }

    ~ncurses_stream();
};

ncurses_stream::~ncurses_stream() {
    m_src.rdbuf(m_srcbuf);
}

class gb_pad {
public:
    bool         m_display_from_bottom;
    int          m_pos;
    WINDOW*      m_win;
    WINDOW*      m_parent_win;
    ncurses_buf& m_nbuf;

    gb_pad(WINDOW* parent_win, ncurses_buf& nbuf);
    ~gb_pad();

    void update_scroll();
    int y_pos();
    void refresh();
    void wait();
    std::string get_string();
};

gb_pad::gb_pad(WINDOW* parent_win, ncurses_buf& nbuf)
    : m_display_from_bottom(false), m_pos(0), m_parent_win(parent_win), m_nbuf(nbuf)
{
    m_win = newpad(GB_DEBUGGER_NWIN_MAX_LINES, getmaxx(stdscr));
    m_nbuf.set_window(m_win);

    nodelay(m_win, true);
    keypad(m_win, true);
    scrollok(m_win, true);
}

gb_pad::~gb_pad() {
    m_nbuf.set_window(m_parent_win);
    if (m_win) delwin(m_win);
}

void gb_pad::update_scroll() {
    m_pos = std::max(getcury(m_win)-getmaxy(stdscr)+1, m_pos);
}

int gb_pad::y_pos() {
    return (m_display_from_bottom ? (getmaxy(stdscr) - 1 - getcury(m_win)) : 0);
}

void gb_pad::refresh() {
    int max_lines = getmaxy(stdscr)-1;
    int max_cols = getmaxx(stdscr);
    prefresh(m_win, m_pos, 0, y_pos(), 0, max_lines, max_cols);
}

void gb_pad::wait() {
    GB_LOGGER(GB_LOG_FATAL) << "Press ENTER to continue...";
    refresh();
    for (int c = wgetch(m_win); c != '\n' && c != '\r'; c = wgetch(m_win)) std::this_thread::sleep_for(std::chrono::milliseconds(GB_DEBUGGER_WAIT_MS));
    wmove(m_win, getcury(m_win), 0);
    wclrtoeol(m_win);
    refresh();
}

std::string gb_pad::get_string() {
    std::string input;
    nodelay(m_win, false);
    for (int c = wgetch(m_win), y = getcury(m_win), x = getcurx(m_win), cur_x = x; c != '\n' && c != '\r'; c = wgetch(m_win), cur_x = getcurx(m_win)) {
        if (c == KEY_BACKSPACE || c == '\b' || c == 127) {
            wdelch(m_win);
            if (cur_x > x) {
                wmove(m_win, y, cur_x-1);
                input.pop_back();
            }
        } else {
            input.push_back(static_cast<char>(c));
            GB_LOGGER(GB_LOG_TRACE) << input.back();
        }
        refresh();
    }
    GB_LOGGER(GB_LOG_TRACE) << std::endl;
    refresh();
    nodelay(m_win, true);
    return input;
}

gb_debugger::gb_debugger(gb_emulator& emulator)
    : m_emulator(emulator), m_key_map(KEY_MAP_INIT), m_command_doc(COMMAND_DOC_LIST), m_frame_cycles(0), m_continue(false)
{
    // Initialize the ncurses library, disable line-buffering and disable character echoing
    // Enable blocking on getch()
    WINDOW* scr = initscr();
    noecho();
    cbreak();
    curs_set(false);

    // Save the cout stream and replace it with a custom one that works with ncurses
    // https://stackoverflow.com/questions/20126649/ncurses-and-ostream-class-method
    m_nstream = std::make_unique<ncurses_stream>(std::cout, scr);

    m_pad = std::make_unique<gb_pad>(scr, m_nstream->m_tbuf);
    m_pad->refresh();
}

gb_debugger::~gb_debugger() {
    // Close the ncurses library
    endwin();
}

void gb_debugger::go() {
    for(int c = wgetch(m_pad->m_win); c != 'q' && m_emulator.m_renderer.is_open(); c = wgetch(m_pad->m_win)) {
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
            m_pad->update_scroll();
        }

        try {
            key_handler_t key_handler = m_key_map.at(c);
            key_handler();
        } catch (const std::out_of_range& oor) {}

        m_pad->refresh();

        if (m_frame_cycles >= 70224) {
            m_frame_cycles = 0;
            m_emulator.m_renderer.update(((m_emulator.m_memory_map.read_byte(GB_LCDC_ADDR) & 0x80) != 0));
        }

        if (!m_continue) std::this_thread::sleep_for(std::chrono::milliseconds(GB_DEBUGGER_WAIT_MS));
    }
}

void gb_debugger::_debugger_help() {
    gb_pad pad (m_pad->m_win, m_nstream->m_tbuf);
    pad.m_display_from_bottom = true;

    // Go through command documentation and print it starting at eos_y
    for (const std::array<std::string, 2>& command : m_command_doc) {
        GB_LOGGER(GB_LOG_TRACE) << command[0] << "\t-" << command[1] << std::endl;
    }

    pad.wait();
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

    m_pad->update_scroll();
}

void gb_debugger::_debugger_dump_registers() {
    m_emulator.m_cpu.dump_registers();
    m_pad->update_scroll();
}

void gb_debugger::_debugger_modify_register() {
    std::string reg;
    uint16_t data = 0;
    bool write = false;

    {
        gb_pad pad (m_pad->m_win, m_nstream->m_tbuf);
        pad.m_display_from_bottom = true;

        GB_LOGGER(GB_LOG_TRACE) << "Register: ";
        pad.refresh();

        // Wait for input
        std::string input = pad.get_string();

        // Search for up to two substrings, the first is the register name and the second is the value to write to it
        size_t pos = input.find_first_of('=', 0);
        write = (pos != std::string::npos) ? true : false;

        try {
            reg = input.substr(0, pos);
            reg.erase(std::remove_if(reg.begin(), reg.end(), [] (char c) { return std::isspace(c); }), reg.end());
            std::transform(reg.begin(), reg.end(), reg.begin(), ::tolower);
        } catch (const std::exception& e) {
            GB_LOGGER(GB_LOG_FATAL) << "gb_debugger::_debugger_modify_register() -- " << e.what() << std::endl;
            pad.wait();
            return;
        }

        if (write) {
            try {
                data = static_cast<uint16_t>(std::stoul(input.substr(pos+1), nullptr, 0));
            } catch (const std::exception& e) {
                GB_LOGGER(GB_LOG_FATAL) << "gb_debugger::_debugger_modify_register() -- " << e.what() << std::endl;
                pad.wait();
                return;
            }
        }

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
            GB_LOGGER(GB_LOG_FATAL) << "gb_debugger::_debugger_modify_register() -- Unknown register: " << reg << std::endl;
            pad.wait();
            return;
        }
    }

    // Read or write data to a register
    if (write) GB_LOGGER(GB_LOG_TRACE) << "write: ";
    else GB_LOGGER(GB_LOG_TRACE) << "read: ";

    GB_LOGGER(GB_LOG_TRACE) << reg << " = " << "0x" << std::setfill('0') << std::setw(4) << std::hex << data << std::endl;
    m_pad->update_scroll();
}

void gb_debugger::_debugger_access_memory() {
    unsigned long addr = 0;
    unsigned long data = 0;
    bool write = false;

    {
        gb_pad pad (m_pad->m_win, m_nstream->m_tbuf);
        pad.m_display_from_bottom = true;

        GB_LOGGER(GB_LOG_TRACE) << "Address: ";
        pad.refresh();

        // Wait for address input
        std::string input = pad.get_string();

        // Search for up to two substrings, the first is the address and the second is the
        // optional data to write to the address separated by a '='
        size_t pos = input.find_first_of('=', 0);
        write = (pos != std::string::npos) ? true : false;

        try {
            addr = std::stoul(input.substr(0, pos), nullptr, 0);
        } catch (const std::exception& e) {
            GB_LOGGER(GB_LOG_TRACE) << "gb_debugger::_debugger_access_memory() -- " << e.what() << std::endl;
            pad.wait();
            return;
        }

        if (write) {
            try {
                data = std::stoul(input.substr(pos+1), nullptr, 0);
            } catch (const std::exception& e) {
                GB_LOGGER(GB_LOG_TRACE) << "gb_debugger::_debugger_access_memory() -- " << e.what() << std::endl;
                pad.wait();
                return;
            }
        }
    }

    // Read or write data to memory
    if (write) {
        m_emulator.m_memory_map.write_byte(static_cast<uint16_t>(addr), static_cast<uint8_t>(data));
        GB_LOGGER(GB_LOG_TRACE) << "write: ";
    } else {
        data = m_emulator.m_memory_map.read_byte(static_cast<uint16_t>(addr));
        GB_LOGGER(GB_LOG_TRACE) << "read: ";
    }

    GB_LOGGER(GB_LOG_TRACE) << "0x" << std::setfill('0') << std::setw(4) << std::hex << addr << " = " << "0x" << std::setfill('0') << std::setw(2) << std::hex << data << std::endl;
    m_pad->update_scroll();
}

void gb_debugger::_debugger_breakpoints() {
    gb_pad pad (m_pad->m_win, m_nstream->m_tbuf);
    pad.m_display_from_bottom = true;

    GB_LOGGER(GB_LOG_TRACE) << "Breakpoints: ";
    pad.refresh();

    // Wait for command input
    std::string input = pad.get_string();

    // Tokenize input on whitespace
    std::istringstream iss (input);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));
    if (tokens.size() >= 1) std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

    unsigned int data = 0;
    auto _try_strtoul = [&pad, &data, tokens] () -> bool {
        if (tokens.size() <= 1) return false;
        try {
            data = static_cast<unsigned int>(std::stoul(tokens[1], nullptr, 0));
        } catch (const std::exception& e) {
            GB_LOGGER(GB_LOG_TRACE) << "gb_debugger::_debugger_breakpoints() -- " << e.what() << std::endl;
            pad.wait();
            return false;
        }
        return true;
    };

    auto _print_breakpoints = [this, &pad] () -> void {
        GB_LOGGER(GB_LOG_TRACE) << "Active breakpoints: ";
        for (auto bp : m_emulator.m_cpu.m_bp.m_breakpoints) {
            GB_LOGGER(GB_LOG_TRACE) << "0x" << std::hex << std::setfill('0') << std::setw(4) << bp << ", ";
        }
        GB_LOGGER(GB_LOG_TRACE) << std::endl;
        pad.wait();
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
        GB_LOGGER(GB_LOG_TRACE) << "gb_debugger::_debugger_breakpoints() -- Unknown command: " << tokens[0] << std::endl;
        pad.wait();
    }
}

void gb_debugger::_debugger_watchpoints() {
    gb_pad pad (m_pad->m_win, m_nstream->m_tbuf);
    pad.m_display_from_bottom = true;

    GB_LOGGER(GB_LOG_TRACE) << "Watchpoints: ";
    pad.refresh();

    // Wait for command input
    std::string input = pad.get_string();

    // Tokenize input on whitespace
    std::istringstream iss (input);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));
    if (tokens.size() >= 1) std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

    unsigned int data = 0;
    auto _try_strtoul = [&pad, &data, tokens] () -> bool {
        if (tokens.size() <= 1) return false;
        try {
            data = static_cast<unsigned int>(std::stoul(tokens[1], nullptr, 0));
        } catch (const std::exception& e) {
            GB_LOGGER(GB_LOG_TRACE) << "gb_debugger::_debugger_watchpoints() -- " << e.what() << std::endl;
            pad.wait();
            return false;
        }
        return true;
    };

    auto _print_watchpoints = [this, &pad] () -> void {
        GB_LOGGER(GB_LOG_TRACE) << "Active watchpoints: ";
        for (auto wp : m_emulator.m_cpu.m_wp.m_breakpoints) {
            GB_LOGGER(GB_LOG_TRACE) << "0x" << std::hex << std::setfill('0') << std::setw(4) << wp << ", ";
        }
        GB_LOGGER(GB_LOG_TRACE) << std::endl;
        pad.wait();
    };

    if (tokens.size() == 0) {
    } else if (tokens[0] == "set" && _try_strtoul()) {
        m_emulator.m_cpu.m_wp.add(data);
        m_emulator.m_cpu.m_wp_enabled = true;
    } else if (tokens[0] == "del" && _try_strtoul()) {
        m_emulator.m_cpu.m_wp.remove(data);
        m_emulator.m_cpu.m_wp_enabled = (m_emulator.m_cpu.m_wp.m_breakpoints.size() != 0);
    } else if (tokens[0] == "clear") {
        m_emulator.m_cpu.m_wp.clear();
        m_emulator.m_cpu.m_wp_enabled = false;
    } else if (tokens[0] == "list") {
        _print_watchpoints();
    } else {
        GB_LOGGER(GB_LOG_TRACE) << "gb_debugger::_debugger_watchpoints() -- Unknown command: " << tokens[0] << std::endl;
        pad.wait();
    }
}

void gb_debugger::_debugger_save_trace() {
    gb_pad pad (m_pad->m_win, m_nstream->m_tbuf);
    pad.m_display_from_bottom = true;

    GB_LOGGER(GB_LOG_TRACE) << "Save file: ";

    // Wait for command input and attempt to open file
    std::string input = pad.get_string();
    std::ofstream file (input);

    if (!file) {
        GB_LOGGER(GB_LOG_TRACE) << "gb_debugger::_debugger_save_trace() -- Can't open file: " << input << std::endl;
        pad.wait();
        return;
    }

    // Go through each line in the window and save it to the file
    char buf[256];
    for (int i = 0, end = getcury(m_pad->m_win); i < end; i++) {
        int c_cnt = mvwinnstr(m_pad->m_win, i, 0, buf, 255);
        file << buf << ((buf[c_cnt-1] == '\n' ? "" : "\n"));
    }

    GB_LOGGER(GB_LOG_TRACE) << "Saved to file: " << input << std::endl;
    pad.wait();
}

void gb_debugger::_debugger_scroll_up_half_pg() {
    // Half page up
    int max_lines = getmaxy(stdscr);
    m_pad->m_pos = std::max(0, m_pad->m_pos-(max_lines/2));
}

void gb_debugger::_debugger_scroll_dn_half_pg() {
    // Half page down
    int max_lines = getmaxy(stdscr);
    m_pad->m_pos = std::min(m_pad->m_pos+(max_lines/2), std::max(0, getcury(m_pad->m_win)-max_lines+1));
}

void gb_debugger::_debugger_scroll_to_start() {
    // To top of buffer
    m_pad->m_pos = 0;
}

void gb_debugger::_debugger_scroll_to_end() {
    // To bottom of buffer
    m_pad->m_pos = std::max(m_pad->m_pos, getcury(m_pad->m_win)-getmaxy(stdscr)+1);
}

void gb_debugger::_debugger_scroll_up_one_line() {
    // 1 line up
    m_pad->m_pos = std::max(0, m_pad->m_pos-1);
}

void gb_debugger::_debugger_scroll_dn_one_line() {
    // 1 line down
    m_pad->m_pos = std::min(m_pad->m_pos+1, std::max(0, getcury(m_pad->m_win)-getmaxy(stdscr)+1));
}

void gb_debugger::_debugger_toggle_continue() {
    m_continue = m_continue ? false : true;
    gb_logger::instance().enable_tracing(true);
}

void gb_debugger::_debugger_toggle_continue_and_tracing() {
    _debugger_toggle_continue();
    gb_logger::instance().enable_tracing(!m_continue);
}
