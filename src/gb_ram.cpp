#include <iostream>

#include "gb_ram.h"

gb_ram::gb_ram(uint16_t start_addr, size_t size)
    : gb_memory_mapped_device(start_addr, size)
{
}

gb_ram::~gb_ram() {
}

void gb_ram::write_byte(uint16_t addr, uint8_t val) {
    if (addr == 0xFF01) {
        m_str.push_back(static_cast<char>(val));
    } else if (addr == 0xFF02 && (val & 0x80) && m_str.back() == '\n') {
        std::cout << m_str << std::endl;
        m_str.clear();
    } else {
        gb_memory_mapped_device::write_byte(addr, val);
    }
}
