#include <streambuf>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#include "gb_debugger.h"

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

const gb_debugger::key_map_t gb_debugger::m_key_map = {
    {'n', &gb_debugger::_debugger_step_once},
    {'r', &gb_debugger::_debugger_dump_registers},
    {'u', &gb_debugger::_debugger_scroll_up_half_pg},
    {'d', &gb_debugger::_debugger_scroll_dn_half_pg},
    {'b', &gb_debugger::_debugger_scroll_up_full_pg},
    {'f', &gb_debugger::_debugger_scroll_dn_full_pg},
    {'G', &gb_debugger::_debugger_scroll_to_start},
    {'g', &gb_debugger::_debugger_scroll_to_end},
    {'c', &gb_debugger::_debugger_toggle_continue},
    {KEY_UP, &gb_debugger::_debugger_scroll_up_one_line},
    {KEY_DOWN, &gb_debugger::_debugger_scroll_dn_one_line}
};

gb_debugger::gb_debugger()
    : gb_emulator(), m_nwin_pos(0), m_nwin_max_lines(10000), m_nwin_lines(0), m_nwin_cols(0)
{
    // Initialize the ncurses library, disable line-buffering and disable character echoing
    // Enable blocking on getch()
    initscr();
    noecho();
    curs_set(false);

    m_nwin_lines = getmaxy(stdscr)-1;
    m_nwin_cols = getmaxx(stdscr);
    m_nwin = newpad(m_nwin_max_lines, m_nwin_cols);
    nodelay(m_nwin, true);
    keypad(m_nwin, true);
    scrollok(m_nwin, true);
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
    for(int c = wgetch(m_nwin); c != 'q'; c = wgetch(m_nwin)) {
        if (m_continue) {
            _debugger_step_once();
        }

        try {
            key_handler_t key_handler = m_key_map.at(c);
            (this->*key_handler)();
        } catch (const std::out_of_range& oor) {}

        prefresh(m_nwin, m_nwin_pos, 0, 0, 0, m_nwin_lines-1, m_nwin_cols);
    }
}

void gb_debugger::_debugger_step_once() {
    int cycles = m_cpu.step();
    m_cycles += static_cast<uint64_t>(cycles);
    m_interrupt_controller.update(cycles);
    m_nwin_pos = std::max(getcury(m_nwin)-m_nwin_lines, m_nwin_pos);
}

void gb_debugger::_debugger_dump_registers() {
    m_cpu.dump_registers();
    m_nwin_pos = std::max(getcury(m_nwin)-m_nwin_lines, m_nwin_pos);
}

void gb_debugger::_debugger_scroll_up_half_pg() {
    // Half page up
    m_nwin_pos = std::max(0, m_nwin_pos-(m_nwin_lines/2));
}

void gb_debugger::_debugger_scroll_dn_half_pg() {
    // Half page down
    m_nwin_pos = std::min(m_nwin_pos+(m_nwin_lines/2), std::max(0, getcury(m_nwin)-m_nwin_lines));
}

void gb_debugger::_debugger_scroll_up_full_pg() {
    // Full page up
    m_nwin_pos = std::max(0, m_nwin_pos-m_nwin_lines);
}

void gb_debugger::_debugger_scroll_dn_full_pg() {
    // Full page down
    m_nwin_pos = std::min(m_nwin_pos+m_nwin_lines, std::max(0, getcury(m_nwin)-m_nwin_lines));
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
}
