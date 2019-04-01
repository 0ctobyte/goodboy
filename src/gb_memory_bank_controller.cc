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

enum gb_mbc_type_t {
    GB_MBC_NONE,
    GB_MBC_TYPE1,
    GB_MBC_TYPE2,
    GB_MBC_TYPE3,
    GB_MBC_TYPE5,
    GB_MBC_UNKNOWN
};

struct gb_cartridge_attributes_t {
    std::string   mbc_str;
    gb_mbc_type_t mbc_type;
    bool          has_ram;
    bool          has_battery;
    bool          has_rtc;
};

using gb_cartridge_attributes_map_t = std::unordered_map<uint8_t, gb_cartridge_attributes_t>;

static gb_cartridge_attributes_map_t cartridge_attr_map =
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

gb_memory_mapped_device_ptr gb_memory_bank_controller::make_mbc(gb_memory_manager& memory_manager, gb_memory_map& memory_map, const std::string& rom_filename) {
    // Read cartridge header and create appropriate RAM/ROM devices and add to the memory map
    std::ifstream rom_file (rom_filename, std::ifstream::binary);

    if (!rom_file) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::make_mbc() - Invalid ROM file: " << rom_filename;
        throw std::runtime_error(sstr.str());
    }

    rom_file.seekg(0, rom_file.end);
    int binsize = static_cast<int>(rom_file.tellg());
    rom_file.seekg(0, rom_file.beg);

    // Add ROM as readable device but not writeable...of course
    gb_rom_ptr rom0 = std::make_shared<gb_rom>(memory_manager, GB_ROM_ADDR, GB_ROM_BANK_SIZE, GB_ROM_BANK_SIZE);
    gb_address_range_t addr_range = rom0->get_address_range();
    memory_map.add_readable_device(rom0, std::get<0>(addr_range), std::get<1>(addr_range));

    // Load first 16KB of ROM with ROM file data
    rom_file.read(reinterpret_cast<char*>(rom0->get_mem()), std::min(GB_ROM_BANK_SIZE, binsize));

    size_t ram_size = 0;
    uint8_t rom_size_code = memory_map.read_byte(0x148);
    int rom_size = 0x8000 << rom_size_code;

    if (rom_size_code > 0x8) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::make_mbc() - Unknown ROM size: " << static_cast<uint16_t>(rom_size_code);
        throw std::runtime_error(sstr.str());
    }

    gb_cartridge_attributes_t cartridge_attr;
    try {
        cartridge_attr = cartridge_attr_map.at(memory_map.read_byte(0x147));
    } catch (std::out_of_range& oor) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::make_mbc - Unknown cartridge type: " << static_cast<uint16_t>(memory_map.read_byte(0x147)) << std::endl;
        throw std::runtime_error(sstr.str());
    }

    try {
        // MBC2 cartridges have 512 x 4 bits on-board RAM
        std::array<size_t, 6> ram_size_list = {0, 0x800, 0x2000, 0x8000, 0x20000, 0x10000};
        ram_size = (cartridge_attr.mbc_type == GB_MBC_TYPE2) ? 0x200 : ram_size_list.at(memory_map.read_byte(0x149));
    } catch (std::out_of_range& oor) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::make_mbc - Unknown RAM size: " << ram_size << std::endl;
        throw std::runtime_error(sstr.str());
    }

    binsize -= rom_file.tellg();

    // Add the 2nd ROM which can be switched to multiple banks
    // rom_size is the size of the entire ROM including bank 0 (i.e. rom0). Subtract 16KB since that's taken care of by rom0
    gb_rom_ptr rom1 = std::make_shared<gb_rom>(memory_manager, GB_ROM_BANK_SIZE, GB_ROM_BANK_SIZE, rom_size - 0x4000);
    addr_range = rom1->get_address_range();
    memory_map.add_readable_device(rom1, std::get<0>(addr_range), std::get<1>(addr_range));

    // Load the next 16KB+ of ROM into the switchable ROM banks
    if (binsize > 0) rom_file.read(reinterpret_cast<char*>(rom1->get_mem()), std::min(rom_size - 0x4000, binsize));

    gb_ram_ptr ram;
    if (cartridge_attr.has_ram || (cartridge_attr.mbc_type == GB_MBC_TYPE2)) {
        // Add 8KB of external RAM (on the cartridge)
        ram = std::make_shared<gb_ram>(memory_manager, GB_RAM_ADDR, GB_RAM_BANK_SIZE, ram_size, cartridge_attr.mbc_type == GB_MBC_TYPE2);
        addr_range = ram->get_address_range();
        memory_map.add_readable_device(ram, std::get<0>(addr_range), std::get<1>(addr_range));
        memory_map.add_writeable_device(ram, std::get<0>(addr_range), std::get<1>(addr_range));
    }

    // Create the RTC device if the cartridge has one
    gb_rtc_ptr rtc;
    if (cartridge_attr.has_rtc) rtc = std::make_shared<gb_rtc>(memory_manager, GB_RAM_ADDR, GB_RAM_BANK_SIZE);

    GB_LOGGER(GB_LOG_INFO) << "Cartridge: " << cartridge_attr.mbc_str << std::endl << "ROM Size: " << std::hex << rom_size << std::endl << "RAM Size: " << std::hex << ram_size << std::endl;

    gb_memory_mapped_device_ptr mbc;
    switch (cartridge_attr.mbc_type) {
        case GB_MBC_NONE: break;
        case GB_MBC_TYPE1: mbc = std::make_shared<gb_mbc1>(memory_manager, memory_map, rom1, ram); break;
        case GB_MBC_TYPE2: mbc = std::make_shared<gb_mbc2>(memory_manager, memory_map, rom1, ram); break;
        case GB_MBC_TYPE3: mbc = std::make_shared<gb_mbc3>(memory_manager, memory_map, rom1, ram, rtc); break;
        case GB_MBC_TYPE5: mbc = std::make_shared<gb_mbc5>(memory_manager, memory_map, rom1, ram); break;
        default:
        {
            std::ostringstream sstr;
            sstr << "gb_memory_bank_controller::make_mbc - Unsupported MBC type: " << cartridge_attr.mbc_str;
            throw std::runtime_error(sstr.str());
        }
    }

    return mbc;
}

gb_mbc1::gb_mbc1(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_rom_ptr rom, gb_ram_ptr ram)
    : gb_memory_mapped_device(memory_manager),
      m_memory_map(memory_map), m_rom(rom), m_ram(ram), m_rom_or_ram_mode(false)
{
    m_start_addr = GB_MBC_ADDR;
    m_size = GB_MBC_SIZE;

    // The MBC doesn't actually need real memory backing it. It only responds to write requests
    // in the RAM address space and forwards read requests to the ROM banks connected to it
    m_mm_start_addr = 0;
}

uint8_t gb_mbc1::read_byte(uint16_t addr) {
    throw std::out_of_range("gb_mbc1::read_byte - read operation not supported");
}

void gb_mbc1::write_byte(uint16_t addr, uint8_t val) {
    uint8_t action = (addr >> 13) & 0x3;
    switch (action) {
        case 0:
        {
            // Enable or disable RAM. Any value with 0xA in the lower nibble will enable RAM, otherwise will disable RAM
            if ((val & 0xf) == 0xa) {
                if (m_ram == nullptr) break;
                gb_address_range_t addr_range = m_ram->get_address_range();
                m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
                m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
            } else {
                m_memory_map.remove_readable_device(GB_RAM_ADDR, GB_RAM_BANK_SIZE);
                m_memory_map.remove_writeable_device(GB_RAM_ADDR, GB_RAM_BANK_SIZE);
            }
        }
        break;
        case 1:
        {
            // MBC1 will translate 0x01 to 0x00 when writing to this register
            // Since this register is the lower 5 bits of the ROM bank number
            // this makes 0x00, 0x20, 0x40 and 0x60 impossible to map
            // (will be translated to 0x01, 0x21, 0x41 and 0x61)
            if ((val & 0x1f) == 0) val = 1;

            // If RAM mode is enabled then only banks 0x01-0x1f can be selected and the upper two bits cleared
            unsigned long cur_bank = m_rom->get_current_bank() + 1;
            cur_bank = (m_rom_or_ram_mode) ? cur_bank & 0x1f : cur_bank;
            m_rom->set_current_bank(((cur_bank & 0xe0) | (val & 0x1f)) - 1);
        }
        break;
        case 2:
        {
            // Depending on the ROM/RAM mode set in register 3, this will either set the low 2 bits of the RAM bank or set the high 2 bits of the ROM bank
            if (m_rom_or_ram_mode) {
                if (m_ram != nullptr) m_ram->set_current_bank(val & 0x3);
            } else {
                unsigned long cur_bank = m_rom->get_current_bank() + 1;
                m_rom->set_current_bank(((cur_bank & 0x1f) | static_cast<unsigned long>(((val & 0x3) << 5))) - 1);
            }
        }
        break;
        case 3: m_rom_or_ram_mode = (val & 0x1) != 0; break;
        default: GB_LOGGER(GB_LOG_WARN) << "gb_mbc1::write_byte - Address not implemented: " << std::hex << addr << " -- " << static_cast<uint16_t>(val) << std::endl; break;
    }
}

gb_mbc2::gb_mbc2(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_rom_ptr rom, gb_ram_ptr ram)
    : gb_memory_mapped_device(memory_manager),
      m_memory_map(memory_map), m_rom(rom), m_ram(ram)
{
    m_start_addr = GB_MBC_ADDR;
    m_size = GB_MBC_SIZE;

    // The MBC doesn't actually need real memory backing it. It only responds to write requests
    // in the RAM address space and forwards read requests to the ROM banks connected to it
    m_mm_start_addr = 0;
}

uint8_t gb_mbc2::read_byte(uint16_t addr) {
    throw std::out_of_range("gb_mbc2::read_byte - read operation not supported");
}

void gb_mbc2::write_byte(uint16_t addr, uint8_t val) {
    uint8_t action = (addr >> 13) & 0x3;
    switch (action) {
        case 0:
        {
            // Enable or disable RAM. Any value with 0xA in the lower nibble will enable RAM, otherwise will disable RAM
            // The least significant bit of the upper address byte must be '0' to enable/disable the RAM
            if (addr & 0x0100) break;
            if ((val & 0xf) == 0xa) {
                gb_address_range_t addr_range = m_ram->get_address_range();
                m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
                m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
            } else {
                m_memory_map.remove_readable_device(GB_RAM_ADDR, GB_RAM_BANK_SIZE);
                m_memory_map.remove_writeable_device(GB_RAM_ADDR, GB_RAM_BANK_SIZE);
            }
        }
        break;
        case 1:
        {
            // The least significant bit of the upper address byte must be '1' to select a ROM bank
            if ((addr & 0x0100) == 0) break;
            if (val == 0) val = 1;
            m_rom->set_current_bank((val & 0xf) - 1);
        }
        break;
        default: GB_LOGGER(GB_LOG_WARN) << "gb_mbc2::write_byte - Address not implemented: " << std::hex << addr << " -- " << static_cast<uint16_t>(val) << std::endl; break;
    }
}

gb_mbc3::gb_mbc3(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_rom_ptr rom, gb_ram_ptr ram, gb_rtc_ptr rtc)
    : gb_memory_mapped_device(memory_manager),
      m_memory_map(memory_map), m_rom(rom), m_ram(ram), m_rtc(rtc), m_rtc_latch(0)
{
    m_start_addr = GB_MBC_ADDR;
    m_size = GB_MBC_SIZE;

    // The MBC doesn't actually need real memory backing it. It only responds to write requests
    // in the RAM address space and forwards read requests to the ROM banks connected to it
    m_mm_start_addr = 0;
}

uint8_t gb_mbc3::read_byte(uint16_t addr) {
    throw std::out_of_range("gb_mbc3::read_byte - read operation not supported");
}

void gb_mbc3::write_byte(uint16_t addr, uint8_t val) {
    uint8_t action = (addr >> 13) & 0x3;
    switch (action) {
        case 0:
        {
            // Enable or disable RAM/RTC. Any value with 0xA in the lower nibble will enable RAM, otherwise will disable RAM
            if ((val & 0xf) == 0xa) {
                if (m_ram == nullptr) break;
                gb_address_range_t addr_range = m_ram->get_address_range();
                m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
                m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
            } else if ((val & 0xf) == 0) {
                m_memory_map.remove_readable_device(GB_RAM_ADDR, GB_RAM_BANK_SIZE);
                m_memory_map.remove_writeable_device(GB_RAM_ADDR, GB_RAM_BANK_SIZE);
            }
        }
        break;
        case 1:
        {
            // Write 7 bits of the ROM bank number
            // A bank number of 0x0 gets translated to 0x1 on MBC3
            if (val == 0) val = 0x1;
            m_rom->set_current_bank((val & 0x7f) - 1);
        }
        break;
        case 2:
        {
            // A value between 0x0-0x3 selects a RAM bank in 0xA000-0xBFFF
            // A value between 0x8-0c selects a RTC register in 0xA000-0xBFFF
            if (val < 0x4 && m_ram != nullptr) {
                gb_address_range_t addr_range = m_ram->get_address_range();
                m_ram->set_current_bank(val);
                m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
                m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
            } else if (val < 0xD && m_rtc != nullptr) {
                gb_address_range_t addr_range = m_rtc->get_address_range();
                m_rtc->set_current_register(val);
                m_memory_map.add_readable_device(m_rtc, std::get<0>(addr_range), std::get<1>(addr_range));
                m_memory_map.add_writeable_device(m_rtc, std::get<0>(addr_range), std::get<1>(addr_range));
            }
        }
        break;
        case 3:
        {
            // Latch RTC registers with clock data from 0 -> 1 transition in this register
            if (m_rtc_latch == 0 && val == 1) m_rtc->update();
            m_rtc_latch = val;
        }
        break;
        default: GB_LOGGER(GB_LOG_WARN) << "gb_mbc3::write_byte - Address not implemented: " << std::hex << addr << " -- " << static_cast<uint16_t>(val) << std::endl; break;
    }
}

gb_mbc5::gb_mbc5(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_rom_ptr rom, gb_ram_ptr ram)
    : gb_memory_mapped_device(memory_manager),
      m_memory_map(memory_map), m_rom(rom), m_ram(ram)
{
    m_start_addr = GB_MBC_ADDR;
    m_size = GB_MBC_SIZE;

    // The MBC doesn't actually need real memory backing it. It only responds to write requests
    // in the RAM address space and forwards read requests to the ROM banks connected to it
    m_mm_start_addr = 0;
}

uint8_t gb_mbc5::read_byte(uint16_t addr) {
    throw std::out_of_range("gb_mbc5::read_byte - read operation not supported");
}

void gb_mbc5::write_byte(uint16_t addr, uint8_t val) {
    uint8_t action = (addr >> 13) & 0x3;
    switch (action) {
        case 0:
        {
            // Enable or disable RAM. Any value with 0xA in the lower nibble will enable RAM, otherwise will disable RAM
            if ((val & 0xf) == 0xa) {
                if (m_ram == nullptr) break;
                gb_address_range_t addr_range = m_ram->get_address_range();
                m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
                m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
            } else {
                m_memory_map.remove_readable_device(GB_RAM_ADDR, GB_RAM_BANK_SIZE);
                m_memory_map.remove_writeable_device(GB_RAM_ADDR, GB_RAM_BANK_SIZE);
            }
        }
        break;
        case 1:
        {
            if ((addr >> 12) & 0x1) {
                // Write the higher 1 bit of the ROM bank number
                unsigned long cur_bank = m_rom->get_current_bank();
                cur_bank = static_cast<unsigned long>((val & 0x1) << 8) | (cur_bank & 0xff);
                if (cur_bank == 0) cur_bank = 0x1;
                m_rom->set_current_bank(cur_bank - 1);
            } else {
                // Write the lower 8 bits of the ROM bank number
                // A bank number of 0x0 gets translated to 0x1 on MBC5
                unsigned long cur_bank = m_rom->get_current_bank();
                cur_bank = (val & 0xff) | (cur_bank & 0xff00);
                if (cur_bank == 0) cur_bank = 0x1;
                m_rom->set_current_bank(cur_bank - 1);
            }
        }
        break;
        case 2:
        {
            // A value between 0x0-0xf selects a RAM bank in 0xA000-0xBFFF
            if (val < 16 && m_ram != nullptr) {
                gb_address_range_t addr_range = m_ram->get_address_range();
                m_ram->set_current_bank(val);
                m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
                m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
            }
        }
        break;
        default: GB_LOGGER(GB_LOG_WARN) << "gb_mbc5::write_byte - Address not implemented: " << std::hex << addr << " -- " << static_cast<uint16_t>(val) << std::endl; break;
    }
}
