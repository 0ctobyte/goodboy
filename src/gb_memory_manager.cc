/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#include "gb_memory_manager.h"

gb_memory_manager::gb_memory_manager()
    : m_mem()
{
}

gb_memory_manager::~gb_memory_manager() {
}

unsigned long gb_memory_manager::allocate(size_t size) {
    unsigned long count = m_mem.size();
    m_mem.resize(size + count, 0);
    return count;
}

uint8_t* gb_memory_manager::get_mem(unsigned long addr) {
    return (m_mem.data() + addr);
}

uint8_t gb_memory_manager::read_byte(unsigned long addr) {
    return m_mem.at(addr);
}

void gb_memory_manager::write_byte(unsigned long addr, uint8_t val) {
    m_mem.at(addr) = val;
}
