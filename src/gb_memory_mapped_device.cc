/*
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDX-License-Identifier: MIT
 */

#include <sstream>
#include <stdexcept>

#include "gb_memory_mapped_device.h"

gb_memory_mapped_device::gb_memory_mapped_device(gb_memory_manager& memory_manager)
    : m_start_addr(0), m_size(0), m_memory_manager(memory_manager)
{
}

gb_memory_mapped_device::gb_memory_mapped_device(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size)
    : m_start_addr(start_addr), m_size(size), m_memory_manager(memory_manager)
{
    m_mm_start_addr = m_memory_manager.allocate(m_size);
}

gb_memory_mapped_device::~gb_memory_mapped_device() {
}

uint8_t* gb_memory_mapped_device::get_mem() {
    return m_memory_manager.get_mem(m_mm_start_addr);
}

gb_address_range_t gb_memory_mapped_device::get_address_range() const {
    return std::make_tuple(m_start_addr, m_size);
}

bool gb_memory_mapped_device::in_range(uint16_t addr) const {
    return ((addr >= m_start_addr) && (addr < (m_start_addr + m_size)));
}

unsigned long gb_memory_mapped_device::translate(uint16_t addr) const {
    if (!in_range(addr)) {
        std::stringstream sstr;
        sstr << "gb_memory_mapped_device::translate - " << std::hex << addr;
        throw std::out_of_range(sstr.str());
    }

    return ((addr - m_start_addr) + m_mm_start_addr);
}

uint8_t gb_memory_mapped_device::read_byte(uint16_t addr) {
    unsigned long taddr = translate(addr);

    return m_memory_manager.read_byte(taddr);
}

void gb_memory_mapped_device::write_byte(uint16_t addr, uint8_t val) {
    unsigned long taddr = translate(addr);

    m_memory_manager.write_byte(taddr, val);
}
