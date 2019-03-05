#include <stdexcept>

#include "gb_memory_mapped_device.h"

gb_memory_mapped_device::gb_memory_mapped_device(uint16_t start_addr, uint16_t end_addr)
    : m_start_addr(start_addr), m_size(end_addr - start_addr), m_mem(new uint8_t[m_size]) {
}

gb_memory_mapped_device::~gb_memory_mapped_device() {
    delete[] m_mem;
}

bool gb_memory_mapped_device::in_range(uint16_t addr) {
    return ((addr >= m_start_addr) && (addr < (m_start_addr+m_size)));
}

uint8_t gb_memory_mapped_device::read_byte(uint16_t addr) {
    if (!in_range(addr)) throw std::out_of_range("gb_memory_mapped_device::read_byte");

    return m_mem[addr];
}

uint16_t gb_memory_mapped_device::read_short(uint16_t addr) {
    uint16_t byte_lo = read_byte(addr);
    uint16_t byte_hi = read_byte(addr+1);
    return static_cast<uint16_t>((byte_hi << 8) | byte_lo);
}

void gb_memory_mapped_device::write_byte(uint16_t addr, uint8_t val) {
    if (!in_range(addr)) throw std::out_of_range("gb_memory_mapped_device::write_byte");

    m_mem[addr] = val;
}
