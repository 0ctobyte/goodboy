#include <sstream>
#include <stdexcept>
#include <algorithm>

#include "gb_ram.h"
#include "gb_io_defs.h"

gb_ram::gb_ram(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size, size_t ram_size, bool _4bit_mode)
    : gb_memory_mapped_device(memory_manager),
      m_ram_size(ram_size), m_num_banks(std::max(1ul, ram_size / GB_RAM_BANK_SIZE)), m_cur_bank(0), m_4bit_mode(_4bit_mode)
{
    m_start_addr = start_addr;
    m_size = size;
    m_mm_start_addr = m_memory_manager.allocate(ram_size);
}

gb_ram::~gb_ram() {
}

unsigned long gb_ram::translate(uint16_t addr) const {
    unsigned long offset = (addr - m_start_addr) + (GB_RAM_BANK_SIZE * m_cur_bank);

    // There's a possibility that the external RAM address space may be larger than the actual RAM size
    // (i.e. only 2KB RAM available in an 8KB (0xA000 - 0xBFFF) address space, or 512x4 bit memory for MBC2)
    // Check to make sure the address offset is within the ram size
    if (!in_range(addr) && m_cur_bank >= m_num_banks && offset > m_ram_size) {
        std::stringstream sstr;
        sstr << "gb_memory_mapped_device::translate - " << std::hex << addr;
        throw std::out_of_range(sstr.str());
    }

    return (offset + m_mm_start_addr);
}

uint8_t gb_ram::read_byte(uint16_t addr) {
    uint8_t data = gb_memory_mapped_device::read_byte(addr);
    return (m_4bit_mode) ? (data & 0xf) : data;
}

unsigned long gb_ram::get_current_bank() const {
    return m_cur_bank;
}

void gb_ram::set_current_bank(unsigned long bank) {
    // Wrap the bank number so it's within the actual supported number of banks for the RAM
    m_cur_bank = bank % m_num_banks;
}
