#ifndef GB_EMULATOR_DEBUGGER_H_
#define GB_EMULATOR_DEBUGGER_H_

#include <map>

#include <ncurses.h>

#include "gb_emulator.h"

class ncurses_stream;

class gb_emulator_debugger : public gb_emulator {
public:
    gb_emulator_debugger(std::string rom_filename);

    virtual ~gb_emulator_debugger();

    virtual bool go();

private:
    typedef void (gb_emulator_debugger::*key_handler_t)();

    ncurses_stream              *m_nstream;
    WINDOW                      *m_nwin;
    int                          m_nwin_pos;
    int                          m_nwin_max_lines;
    int                          m_nwin_lines;
    int                          m_nwin_cols;
    std::map<int, key_handler_t> m_key_map = {
        {'n', &gb_emulator_debugger::_debugger_step_once},
        {'r', &gb_emulator_debugger::_debugger_dump_registers},
        {'u', &gb_emulator_debugger::_debugger_scroll_up_half_pg},
        {'d', &gb_emulator_debugger::_debugger_scroll_dn_half_pg},
        {'b', &gb_emulator_debugger::_debugger_scroll_up_full_pg},
        {'f', &gb_emulator_debugger::_debugger_scroll_dn_full_pg},
        {'G', &gb_emulator_debugger::_debugger_scroll_to_start},
        {'g', &gb_emulator_debugger::_debugger_scroll_to_end},
        {KEY_UP, &gb_emulator_debugger::_debugger_scroll_up_one_line},
        {KEY_DOWN, &gb_emulator_debugger::_debugger_scroll_dn_one_line}
    };

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
};

#endif // GB_EMULATOR_DEBUGGER_H_
