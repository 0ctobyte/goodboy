/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#ifndef GB_DEBUGGER_H_
#define GB_DEBUGGER_H_

#include <unordered_map>
#include <functional>

#include "gb_emulator.h"
#include "gb_ppu.h"

class ncurses_stream;
class gb_pad;

using gb_pad_ptr = std::unique_ptr<gb_pad>;

class gb_debugger {
public:
    gb_debugger(gb_emulator& emulator);
    ~gb_debugger();

    void go();

private:
    using key_handler_t      = std::function<void()>;
    using key_map_t          = std::unordered_map<int, key_handler_t>;
    using ncurses_stream_ptr = std::unique_ptr<ncurses_stream>;
    using command_doc_t      = std::vector<std::array<std::string, 2>>;

    gb_emulator&        m_emulator;
    const key_map_t     m_key_map;
    const command_doc_t m_command_doc;
    ncurses_stream_ptr  m_nstream;
    gb_pad_ptr          m_pad;
    int                 m_frame_cycles;
    bool                m_continue;
    gb_ppu_ptr          m_ppu;

    void _debugger_help();
    void _debugger_step_once();
    void _debugger_dump_registers();
    void _debugger_modify_register();
    void _debugger_access_memory();
    void _debugger_breakpoints();
    void _debugger_watchpoints();
    void _debugger_save_trace();
    void _debugger_sprite_viewer();
    void _debugger_tile_map_viewer();
    void _debugger_scroll_up_half_pg();
    void _debugger_scroll_dn_half_pg();
    void _debugger_scroll_to_start();
    void _debugger_scroll_to_end();
    void _debugger_scroll_up_one_line();
    void _debugger_scroll_dn_one_line();
    void _debugger_toggle_continue();
    void _debugger_toggle_continue_and_tracing();
};

#endif // GB_DEBUGGER_H_
