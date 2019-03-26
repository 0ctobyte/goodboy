#include <stdexcept>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "gb_memory_bank_controller.h"
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

struct gb_cartridge_attr_t {
    std::string   mbc_str;
    gb_mbc_type_t mbc_type;
    bool          has_ram;
    bool          has_battery;
    bool          has_rtc;
};

static std::unordered_map<uint8_t, gb_cartridge_attr_t>  cartridge_type_map =
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
      m_has_ram(false), m_has_battery(false), m_has_rtc(false),
      //m_ram_enable(false), m_ram_rom_mode(false), m_cur_rom_bank(0), m_cur_ram_bank(0),
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

    gb_cartridge_attr_t cartridge_attr;
    size_t ram_size = 0;
    uint8_t rom_size_code = m_memory_map.read_byte(0x148);
    int rom_size = 0x8000 << rom_size_code;

    if (rom_size_code > 0x8) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::load_rom() - Unknown ROM size: " << static_cast<uint16_t>(rom_size_code);
        throw std::runtime_error(sstr.str());
    }

    try {
        cartridge_attr = cartridge_type_map.at(m_memory_map.read_byte(0x147));
        m_has_ram = cartridge_attr.has_ram;
        m_has_battery = cartridge_attr.has_battery;
        m_has_rtc = cartridge_attr.has_rtc;
    } catch (std::out_of_range& oor) {
        std::ostringstream sstr;
        sstr << "gb_memory_bank_controller::load_rom - Unknown cartridge type: " << static_cast<uint16_t>(m_memory_map.read_byte(0x147)) << std::endl;
        throw std::runtime_error(sstr.str());
    }

    try {
        // MBC2 cartridges have 512 x 4 bits on-board RAM
        std::array<size_t, 6> ram_size_list = {0, 0x800, 0x2000, 0x8000, 0x20000, 0x10000};
        ram_size = (cartridge_attr.mbc_type == GB_MBC_TYPE2) ? 0x200 : ram_size_list.at(m_memory_map.read_byte(0x149));
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

    if (m_has_ram || (cartridge_attr.mbc_type == GB_MBC_TYPE2)) {
        // Add 8KB of external RAM (on the cartridge)
        m_ram = std::make_shared<gb_ram>(m_memory_manager, 0xA000, GB_RAM_BANK_SIZE, ram_size);
        addr_range = m_ram->get_address_range();
        m_memory_map.add_readable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
        m_memory_map.add_writeable_device(m_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    }

    GB_LOGGER(GB_LOG_INFO) << "Cartridge: " << cartridge_attr.mbc_str << std::endl << "ROM Size: " << std::hex << rom_size << std::endl << "RAM Size: " << std::hex << ram_size << std::endl;
}

uint8_t gb_memory_bank_controller::read_byte(uint16_t addr) {
    throw std::out_of_range("gb_memory_bank_controller::read_byte - read operation not supported");
}

void gb_memory_bank_controller::write_byte(uint16_t addr, uint8_t val) {
    // Trap writes to ROM here and set appropriate flags and calls to rom/ram devices
    GB_LOGGER(GB_LOG_WARN) << "gb_memory_bank_controller::write_byte - Address not implemented: " << std::hex << addr << " -- " << static_cast<uint16_t>(val) << std::endl;
}
