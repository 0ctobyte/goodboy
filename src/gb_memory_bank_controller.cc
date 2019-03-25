#include <stdexcept>
#include <fstream>
#include <sstream>

#include "gb_memory_bank_controller.h"
#include "gb_logger.h"

// The memory bank controller responds to writes in the following address space
#define GB_MBC_ADDR (0x0)
#define GB_MBC_SIZE (0x8000)

gb_memory_bank_controller::gb_memory_bank_controller(gb_memory_manager& memory_manager, gb_memory_map& memory_map)
    : gb_memory_mapped_device(memory_manager),
      //m_has_ram(false), m_has_battery(false), m_has_timer(false), m_ram_enable(false), m_ram_rom_mode(false), m_cur_rom_bank(0), m_cur_ram_bank(0),
      m_memory_map(memory_map), m_rom0(), m_rom1(), m_ram()
{
    m_start_addr = GB_MBC_ADDR;
    m_size = GB_MBC_SIZE;

    // The MBC doesn't actually need real memory backing it. It only responds to write requests
    // in the RAM address space and forwards read requests to the ROM banks connected to it
    m_mm_start_addr = 0;
}

gb_memory_bank_controller::~gb_memory_bank_controller() {
}

void gb_memory_bank_controller::load_rom(const std::string& rom_filename) {
    // Read cartridge header and create appropriate RAM/ROM devices and add to the memory map
    std::ifstream rom_file (rom_filename, std::ifstream::binary);

    if (!rom_file) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::load_rom() - Invalid ROM file: " << rom_filename;
        throw std::runtime_error(sstr.str());
    }

    rom_file.seekg(0, rom_file.end);
    int binsize = static_cast<int>(rom_file.tellg());
    rom_file.seekg(0, rom_file.beg);

    // Add ROM as readable device but not writeable...of course
    m_rom0 = std::make_shared<gb_rom>(m_memory_manager, 0x0000, GB_ROM_BANK_SIZE, GB_ROM_BANK_SIZE);
    gb_address_range_t addr_range = m_rom0->get_address_range();
    m_memory_map.add_readable_device(m_rom0, std::get<0>(addr_range), std::get<1>(addr_range));

    // Load first 16KB of ROM with ROM file data
    rom_file.read(reinterpret_cast<char*>(m_rom0->get_mem()), std::min(GB_ROM_BANK_SIZE, binsize));

    unsigned long rom_size = 0x4000;
    binsize -= rom_file.tellg();

    // Add the 2nd ROM which can be switched to multiple banks
    m_rom1 = std::make_shared<gb_rom>(m_memory_manager, GB_ROM_BANK_SIZE, GB_ROM_BANK_SIZE, rom_size);
    addr_range = m_rom1->get_address_range();
    m_memory_map.add_readable_device(m_rom1, std::get<0>(addr_range), std::get<1>(addr_range));

    // Load the next 16KB+ of ROM into the switchable ROM banks
    if (binsize > 0) rom_file.read(reinterpret_cast<char*>(m_rom1->get_mem()), std::min(GB_ROM_BANK_SIZE, binsize));

    // Add 8KB of external RAM (on the cartridge)
    m_ram = std::make_shared<gb_ram>(m_memory_manager, 0xA000, 0x2000, 0x2000);
    addr_range = m_ram->get_address_range();
    m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
}

uint8_t gb_memory_bank_controller::read_byte(uint16_t addr) {
    throw std::out_of_range("gb_memory_bank_controller::read_byte - read operation not supported");
}

void gb_memory_bank_controller::write_byte(uint16_t addr, uint8_t val) {
    // Trap writes to ROM here and set appropriate flags and calls to rom/ram devices
    GB_LOGGER(GB_LOG_WARN) << "gb_memory_bank_controller::write_byte - Address not implemented: " << std::hex << addr << " -- " << static_cast<uint16_t>(val) << std::endl;
}
