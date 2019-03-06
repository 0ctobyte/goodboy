#include <iostream>

#include "gb_test_ram.h"

gb_test_ram::gb_test_ram() {
}

gb_test_ram::~gb_test_ram() {
}

uint8_t gb_test_ram::read_byte(uint16_t addr) {
    //std::cout << "R - " << std::hex << addr << " : " << static_cast<uint32_t>(m_mem[addr]) << std::endl;
    return m_mem[addr];
}

void gb_test_ram::write_byte(uint16_t addr, uint8_t val) {
    //std::cout << "W - " << std::hex << addr << " : " << static_cast<uint32_t>(val) << std::endl;
    if (addr == 0xFF01) {
        m_str.push_back(static_cast<char>(val));
    } else if (addr == 0xFF02 && (val & 0x80) && m_str.back() == '\n') {
        std::cout << m_str << std::endl;
        m_str.clear();
    } else {
        m_mem[addr] = val;
    }
}
