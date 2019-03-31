#include <sstream>
#include <stdexcept>

#include "gb_rom.h"
#include "gb_io_defs.h"
#include "gb_logger.h"

gb_rom::gb_rom(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size, size_t rom_size)
    :  gb_memory_mapped_device(memory_manager),
       m_num_banks(rom_size / GB_ROM_BANK_SIZE), m_cur_bank(0)
{
    m_start_addr = start_addr;
    m_size = size;
    m_mm_start_addr = m_memory_manager.allocate(rom_size);
}

gb_rom::~gb_rom() {
}

unsigned long gb_rom::translate(uint16_t addr) const {
    if (!in_range(addr) && m_cur_bank >= m_num_banks) {
        std::stringstream sstr;
        sstr << "gb_memory_mapped_device::translate - " << std::hex << addr;
        throw std::out_of_range(sstr.str());
    }

    return ((addr - m_start_addr) + (GB_ROM_BANK_SIZE * m_cur_bank) + m_mm_start_addr);
}

void gb_rom::write_byte(uint16_t addr, uint8_t val) {
    GB_LOGGER(GB_LOG_WARN) << "gb_rom::write_byte - Attempting to write to read-only memory: " << std::hex << addr << " : " << std::hex << static_cast<uint16_t>(val) << std::endl;
}

unsigned long gb_rom::get_current_bank() const {
    return m_cur_bank;
}

void gb_rom::set_current_bank(unsigned long bank) {
    // Wrap the bank number so it's within the actual supported number of banks for the ROM
    m_cur_bank = bank % m_num_banks;
}
