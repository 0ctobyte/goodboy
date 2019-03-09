#include <iostream>
#include <fstream>

#include "gb_emulator.h"

#include "gb_ram.h"

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

    gb_ram* ram = new gb_ram(0x0000, 0x10000);

    rom_file.read(reinterpret_cast<char*>(ram->get_mem()), m_binsize);

    rom_file.close();

    m_device_list.push_back(static_cast<gb_memory_mapped_device*>(ram));
    m_memory_map.add_readable_device(ram, 0x0000, 0x10000);
    m_memory_map.add_writeable_device(ram, 0x0000, 0x10000);

    return true;
}

bool gb_emulator::go() {
    unsigned instruction_count = 0;
    while (1) {
        m_cycles += m_cpu.step();
        instruction_count++;
    }
}
