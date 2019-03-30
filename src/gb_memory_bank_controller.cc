#include <stdexcept>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "gb_memory_bank_controller.h"
#include "gb_io_defs.h"
#include "gb_logger.h"

// The memory bank controller responds to writes in the following address space
#define GB_MBC_ADDR (0x0)
#define GB_MBC_SIZE (0x8000)

gb_memory_bank_controller::gb_cartridge_attributes_map_t gb_memory_bank_controller::m_cartridge_attr_map =
{
    {0x00, {"ROM ONLY", GB_MBC_NONE, false, false, false}},
    {0x01, {"MBC1", GB_MBC_TYPE1, false, false, false}},
    {0x02, {"MBC1+RAM", GB_MBC_TYPE1, true, false, false}},
    {0x03, {"MBC1+RAM+BATTERY", GB_MBC_TYPE1, true, true, false}},
    {0x05, {"MBC2", GB_MBC_TYPE2, false, false, false}},
    {0x06, {"MBC2+BATTERY", GB_MBC_TYPE2, false, true, false}},
    {0x08, {"ROM+RAM", GB_MBC_NONE, true, false, false}},
    {0x09, {"ROM+RAM+BATTERY", GB_MBC_NONE, true, true, false}},
    {0x0F, {"MBC3+RTC+BATTERY", GB_MBC_TYPE3, false, true, true}},
    {0x10, {"MBC3+RTC+RAM+BATTERY", GB_MBC_TYPE3, true, true, true}},
    {0x11, {"MBC3", GB_MBC_TYPE3, false, false, false}},
    {0x12, {"MBC3+RAM", GB_MBC_TYPE3, true, false, false}},
    {0x13, {"MBC3+RAM+BATTERY", GB_MBC_TYPE3, true, true, false}},
    {0x19, {"MBC5", GB_MBC_TYPE5, false, false, false}},
    {0x1A, {"MBC5+RAM", GB_MBC_TYPE5, true, false, false}},
    {0x1B, {"MBC5+RAM+BATTERY", GB_MBC_TYPE5, true, true, false}},
    {0x1C, {"MBC5+RUMBLE", GB_MBC_TYPE5, false, false, false}},
    {0x1D, {"MBC+RUMBLE+RAM", GB_MBC_TYPE5, true, false, false}},
    {0x1E, {"MBC+RUMBLE+RAM+BATTERY", GB_MBC_TYPE5, true, true, false}}
};

gb_memory_bank_controller::gb_memory_bank_controller(gb_memory_manager& memory_manager, gb_memory_map& memory_map)
    : gb_memory_mapped_device(memory_manager),
      m_memory_map(memory_map), m_cartridge_attr(), m_mbc_actions(), m_rom_or_ram_mode(false), m_rom0(), m_rom1(), m_ram()
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
    m_rom0 = std::make_shared<gb_rom>(m_memory_manager, GB_ROM_ADDR, GB_ROM_BANK_SIZE, GB_ROM_BANK_SIZE);
    gb_address_range_t addr_range = m_rom0->get_address_range();
    m_memory_map.add_readable_device(m_rom0, std::get<0>(addr_range), std::get<1>(addr_range));

    // Load first 16KB of ROM with ROM file data
    rom_file.read(reinterpret_cast<char*>(m_rom0->get_mem()), std::min(GB_ROM_BANK_SIZE, binsize));

    size_t ram_size = 0;
    uint8_t rom_size_code = m_memory_map.read_byte(0x148);
    int rom_size = 0x8000 << rom_size_code;

    if (rom_size_code > 0x8) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::load_rom() - Unknown ROM size: " << static_cast<uint16_t>(rom_size_code);
        throw std::runtime_error(sstr.str());
    }

    try {
        m_cartridge_attr = m_cartridge_attr_map.at(m_memory_map.read_byte(0x147));
    } catch (std::out_of_range& oor) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::load_rom - Unknown cartridge type: " << static_cast<uint16_t>(m_memory_map.read_byte(0x147)) << std::endl;
        throw std::runtime_error(sstr.str());
    }

    try {
        // MBC2 cartridges have 512 x 4 bits on-board RAM
        std::array<size_t, 6> ram_size_list = {0, 0x800, 0x2000, 0x8000, 0x20000, 0x10000};
        ram_size = (m_cartridge_attr.mbc_type == GB_MBC_TYPE2) ? 0x200 : ram_size_list.at(m_memory_map.read_byte(0x149));
    } catch (std::out_of_range& oor) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::load_rom - Unknown RAM size: " << ram_size << std::endl;
        throw std::runtime_error(sstr.str());
    }

    binsize -= rom_file.tellg();

    // Add the 2nd ROM which can be switched to multiple banks
    // rom_size is the size of the entire ROM including bank 0 (i.e. m_rom0). Subtract 16KB since that's taken care of by m_rom0
    m_rom1 = std::make_shared<gb_rom>(m_memory_manager, GB_ROM_BANK_SIZE, GB_ROM_BANK_SIZE, rom_size - 0x4000);
    addr_range = m_rom1->get_address_range();
    m_memory_map.add_readable_device(m_rom1, std::get<0>(addr_range), std::get<1>(addr_range));

    // Load the next 16KB+ of ROM into the switchable ROM banks
    if (binsize > 0) rom_file.read(reinterpret_cast<char*>(m_rom1->get_mem()), std::min(rom_size - 0x4000, binsize));

    if (m_cartridge_attr.has_ram || (m_cartridge_attr.mbc_type == GB_MBC_TYPE2)) {
        // Add 8KB of external RAM (on the cartridge)
        m_ram = std::make_shared<gb_ram>(m_memory_manager, GB_RAM_ADDR, GB_RAM_BANK_SIZE, ram_size);
        addr_range = m_ram->get_address_range();
        m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
        m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    }

    GB_LOGGER(GB_LOG_INFO) << "Cartridge: " << m_cartridge_attr.mbc_str << std::endl << "ROM Size: " << std::hex << rom_size << std::endl << "RAM Size: " << std::hex << ram_size << std::endl;

    // Register actions based on cartridge MBC type
    switch (m_cartridge_attr.mbc_type) {
        case GB_MBC_TYPE1:
        {
            m_mbc_actions[0] = std::bind(&gb_memory_bank_controller::mbc1_enable_ram, this, std::placeholders::_1);
            m_mbc_actions[1] = std::bind(&gb_memory_bank_controller::mbc1_set_rom_bank_lo, this, std::placeholders::_1);
            m_mbc_actions[2] = std::bind(&gb_memory_bank_controller::mbc1_set_rom_or_ram_bank, this, std::placeholders::_1);
            m_mbc_actions[3] = std::bind(&gb_memory_bank_controller::mbc1_set_rom_or_ram_mode, this, std::placeholders::_1);
        }
        break;
        default: break;
    }
}

uint8_t gb_memory_bank_controller::read_byte(uint16_t addr) {
    throw std::out_of_range("gb_memory_bank_controller::read_byte - read operation not supported");
}

void gb_memory_bank_controller::write_byte(uint16_t addr, uint8_t val) {
    // Trap writes to ROM here and set appropriate flags and calls to rom/ram devices
    uint8_t action = (addr >> 13) & 0x3;
    try {
        (m_mbc_actions.at(action))(val);
    } catch (const std::exception& e) {
        GB_LOGGER(GB_LOG_WARN) << "gb_memory_bank_controller::write_byte - Address not implemented: " << std::hex << addr << " -- " << static_cast<uint16_t>(val) << std::endl;
    }
}

void gb_memory_bank_controller::mbc1_enable_ram(uint8_t data) {
    gb_address_range_t addr_range = m_ram->get_address_range();
    if ((data & 0xf) == 0xa) {
        m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
        m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    } else {
        m_memory_map.remove_readable_device(std::get<0>(addr_range), std::get<1>(addr_range));
        m_memory_map.remove_writeable_device(std::get<0>(addr_range), std::get<1>(addr_range));
    }
}

void gb_memory_bank_controller::mbc1_set_rom_bank_lo(uint8_t data) {
    // MBC1 will translate 0x01 to 0x00 when writing to this register
    // Since this register is the lower 5 bits of the ROM bank number
    // this makes 0x00, 0x20, 0x40 and 0x60 impossible to map
    // (will be translated to 0x01, 0x21, 0x41 and 0x61)
    if ((data & 0x1f) == 0) data = 1;

    // If RAM mode is enabled then only banks 0x01-0x1f can be selected and the upper two bits cleared
    unsigned long cur_bank = m_rom1->get_current_bank() + 1;
    cur_bank = (m_rom_or_ram_mode) ? cur_bank & 0x1f : cur_bank;
    m_rom1->set_current_bank(((cur_bank & 0xe0) | (data & 0x1f)) - 1);
}

void gb_memory_bank_controller::mbc1_set_rom_bank_hi(uint8_t data) {
    unsigned long cur_bank = m_rom1->get_current_bank() + 1;
    m_rom1->set_current_bank(((cur_bank & 0x1f) | static_cast<unsigned long>(((data & 0x3) << 5))) - 1);
}

void gb_memory_bank_controller::mbc1_set_ram_bank(uint8_t data) {
    m_ram->set_current_bank(data & 0x3);
}

void gb_memory_bank_controller::mbc1_set_rom_or_ram_bank(uint8_t data) {
    if (m_rom_or_ram_mode) mbc1_set_ram_bank(data);
    else mbc1_set_rom_bank_hi(data);
}

void gb_memory_bank_controller::mbc1_set_rom_or_ram_mode(uint8_t data) {
    m_rom_or_ram_mode = (data & 0x1) != 0;
}
