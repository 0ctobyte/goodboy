/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#include "gb_interrupt_source.h"

gb_interrupt_source::gb_interrupt_source(uint16_t jump_address, uint8_t flag_bit)
    : m_jump_address(jump_address), m_flag_bit(flag_bit)
{
}

gb_interrupt_source::~gb_interrupt_source() {
}

uint16_t gb_interrupt_source::get_jump_address() const {
    return m_jump_address;
}

uint8_t gb_interrupt_source::get_flag_mask() const {
    return static_cast<uint8_t>(1 << m_flag_bit);
}

bool gb_interrupt_source::update(int cycles) {
    return false;
}
