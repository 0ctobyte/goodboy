#include <fstream>

#include "gb_emulator.h"
#include "gb_rom.h"
#include "gb_ram.h"
#include "gb_serial_io_device.h"

gb_emulator::gb_emulator()
    : m_memory_map(), m_cpu(m_memory_map), m_cycles(0) {
}

gb_emulator::~gb_emulator() {
}

bool gb_emulator::load_rom(std::string rom_filename) {
    std::ifstream rom_file (rom_filename, std::ifstream::binary);

    if (!rom_file) return false;

    rom_file.seekg(0, rom_file.end);
    m_binsize = std::min(static_cast<int>(rom_file.tellg()), 0x10000);
    rom_file.seekg(0, rom_file.beg);

    // First 32KB of ROM
    gb_rom* rom = new gb_rom(0x0000, 0x8000);

    rom_file.read(reinterpret_cast<char*>(rom->get_mem()), m_binsize);

    rom_file.close();

    // Add ROM as readable device but not writeable...of course
    m_device_list.push_back(static_cast<gb_memory_mapped_device*>(rom));
    m_memory_map.add_readable_device(rom, 0x0000, 0x8000);

    // Add 8KB of external RAM (on the cartridge)
    gb_ram* ext_ram = new gb_ram(0xA000, 0x2000);
    m_device_list.push_back(static_cast<gb_memory_mapped_device*>(ext_ram));
    m_memory_map.add_readable_device(ext_ram, 0xA000, 0x2000);
    m_memory_map.add_writeable_device(ext_ram, 0xA000, 0x2000);

    // Another 8KB of work RAM (on the gameboy)
    gb_ram* work_ram = new gb_ram(0xC000, 0x2000);
    m_device_list.push_back(static_cast<gb_memory_mapped_device*>(work_ram));
    m_memory_map.add_readable_device(work_ram, 0xC000, 0x2000);
    m_memory_map.add_writeable_device(work_ram, 0xC000, 0x2000);

    // Add Serial IO device for printing to terminal
    gb_serial_io_device* sio = new gb_serial_io_device();
    gb_address_range_t sio_addr_range = sio->get_address_range();
    m_device_list.push_back(static_cast<gb_memory_mapped_device*>(sio));
    m_memory_map.add_readable_device(sio, std::get<0>(sio_addr_range), std::get<1>(sio_addr_range));
    m_memory_map.add_writeable_device(sio, std::get<0>(sio_addr_range), std::get<1>(sio_addr_range));

    // Add 128 bytes of high RAM (used for stack and temp variables)
    gb_ram* high_ram = new gb_ram(0xFF80, 0x80);
    m_device_list.push_back(static_cast<gb_memory_mapped_device*>(high_ram));
    m_memory_map.add_readable_device(high_ram, 0xFF80, 0x80);
    m_memory_map.add_writeable_device(high_ram, 0xFF80, 0x80);

    return true;
}

bool gb_emulator::go() {
    unsigned instruction_count = 0;
    while (1) {
        m_cycles += m_cpu.step();
        instruction_count++;
    }
}
