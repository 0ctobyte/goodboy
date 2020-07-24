/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#include "gb_joypad.h"
#include "gb_io_defs.h"

#define GB_JOYPAD_JUMP_ADDR (0x0060)
#define GB_JOYPAD_FLAG_BIT  (4)

gb_joypad::gb_joypad(gb_memory_manager& memory_manager, gb_input& input)
    : gb_memory_mapped_device(memory_manager, GB_JOYPAD_P1_ADDR, 1),
      gb_interrupt_source(GB_JOYPAD_JUMP_ADDR, GB_JOYPAD_FLAG_BIT),
      m_input(input)
{
    // Reset the joypad register (should be 1)
    gb_memory_mapped_device::write_byte(GB_JOYPAD_P1_ADDR, 0xFF);
}

void gb_joypad::write_byte(uint16_t addr, uint8_t val) {
    if (addr == GB_JOYPAD_P1_ADDR) {
        // Don't overwrite bits[3:0] which are the Joypad input selects
        // bits[7:6] are reserved and should be read-as-one
        uint8_t p1 = read_byte(GB_JOYPAD_P1_ADDR);
        val = (p1 & 0xf) | (val & 0x30) | 0xc0;
    }
    gb_memory_mapped_device::write_byte(addr, val);
}

bool gb_joypad::update(int cycles) {
    bool interrupt = false;
    uint8_t p1 = read_byte(GB_JOYPAD_P1_ADDR);
    uint8_t prev_p1 = p1;
    p1 |= 0xcf;

    if (m_input.is_pressed(GB_BUTTON_DOWN)   && (p1 & 0x10) == 0) p1 &= ~0x8;
    if (m_input.is_pressed(GB_BUTTON_UP)     && (p1 & 0x10) == 0) p1 &= ~0x4;
    if (m_input.is_pressed(GB_BUTTON_LEFT)   && (p1 & 0x10) == 0) p1 &= ~0x2;
    if (m_input.is_pressed(GB_BUTTON_RIGHT)  && (p1 & 0x10) == 0) p1 &= ~0x1;
    if (m_input.is_pressed(GB_BUTTON_START)  && (p1 & 0x20) == 0) p1 &= ~0x8;
    if (m_input.is_pressed(GB_BUTTON_SELECT) && (p1 & 0x20) == 0) p1 &= ~0x4;
    if (m_input.is_pressed(GB_BUTTON_B)      && (p1 & 0x20) == 0) p1 &= ~0x2;
    if (m_input.is_pressed(GB_BUTTON_A)      && (p1 & 0x20) == 0) p1 &= ~0x1;

    // The joypad interrupt is triggered only if one bits [3:0] go from 1 to 0
    interrupt = ((prev_p1 & 0xf) & ~(p1 & 0xf));

    gb_memory_mapped_device::write_byte(GB_JOYPAD_P1_ADDR, p1);

    return interrupt;
}
