#include <fstream>

#include "gb_emulator.h"
#include "gb_rom.h"
#include "gb_ram.h"
#include "gb_serial_io.h"

gb_emulator::gb_emulator()
    : m_memory_map(),
      m_cpu(m_memory_map),
      m_interrupt_controller(m_memory_map, m_cpu),
      m_cycles(0)
{
}

gb_emulator::~gb_emulator() {
}

bool gb_emulator::load_rom(const std::string& rom_filename) {
    std::ifstream rom_file (rom_filename, std::ifstream::binary);

    if (!rom_file) return false;

    rom_file.seekg(0, rom_file.end);
    int binsize = std::min(static_cast<int>(rom_file.tellg()), 0x10000);
    rom_file.seekg(0, rom_file.beg);

    // First 32KB of ROM
    gb_rom_ptr rom = std::make_shared<gb_rom>(0x0000, 0x8000);

    rom_file.read(reinterpret_cast<char*>(rom->get_mem()), binsize);

    rom_file.close();

    // Add ROM as readable device but not writeable...of course
    gb_address_range_t addr_range = rom->get_address_range();
    m_memory_map.add_readable_device(rom, std::get<0>(addr_range), std::get<1>(addr_range));

    // Add 8KB of external RAM (on the cartridge)
    gb_ram_ptr ext_ram = std::make_shared<gb_ram>(0xA000, 0x2000);
    addr_range = ext_ram->get_address_range();
    m_memory_map.add_readable_device(ext_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(ext_ram, std::get<0>(addr_range), std::get<1>(addr_range));

    // Another 8KB of work RAM (on the gameboy)
    gb_ram_ptr work_ram = std::make_shared<gb_ram>(0xC000, 0x2000);
    addr_range = work_ram->get_address_range();
    m_memory_map.add_readable_device(work_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(work_ram, std::get<0>(addr_range), std::get<1>(addr_range));

    // Add Serial IO device for printing to terminal
    gb_serial_io_ptr sio = std::make_shared<gb_serial_io>();
    addr_range = sio->get_address_range();
    m_memory_map.add_readable_device(sio, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(sio, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(sio);

    // Add 128 bytes of high RAM (used for stack and temp variables)
    gb_ram_ptr high_ram = std::make_shared<gb_ram>(0xFF80, 0x80);
    addr_range = high_ram->get_address_range();
    m_memory_map.add_readable_device(high_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(high_ram, std::get<0>(addr_range), std::get<1>(addr_range));

    return true;
}

void gb_emulator::go() {
    while (1) {
        int cycles = m_cpu.step();
        m_cycles += static_cast<uint64_t>(cycles);
        m_interrupt_controller.update(cycles);
    }
}
