#ifndef GB_DEBUGGER_H_
#define GB_DEBUGGER_H_

#include <unordered_map>

#include <ncurses.h>

#include "gb_emulator.h"

class ncurses_stream;

class gb_debugger : public gb_emulator {
public:
    gb_debugger();
    virtual ~gb_debugger() override;

    virtual void go() override;

private:
    using key_handler_t      = void (gb_debugger::*)();
    using key_map_t          = std::unordered_map<int, key_handler_t>;
    using ncurses_stream_ptr = std::unique_ptr<ncurses_stream>;

    static const key_map_t       m_key_map;
    ncurses_stream_ptr           m_nstream;
    WINDOW*                      m_nwin;
    int                          m_nwin_pos;
    int                          m_nwin_max_lines;
    int                          m_nwin_lines;
    int                          m_nwin_cols;
    bool                         m_continue;

    void _debugger_step_once();
    void _debugger_dump_registers();
    void _debugger_scroll_up_half_pg();
    void _debugger_scroll_dn_half_pg();
    void _debugger_scroll_up_full_pg();
    void _debugger_scroll_dn_full_pg();
    void _debugger_scroll_to_start();
    void _debugger_scroll_to_end();
    void _debugger_scroll_up_one_line();
    void _debugger_scroll_dn_one_line();
    void _debugger_toggle_continue();
};

#endif // GB_DEBUGGER_H_
