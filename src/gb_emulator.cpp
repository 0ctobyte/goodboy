#include <iostream>
#include <fstream>

#include "gb_emulator.h"

#include "gb_test_ram.h"

gb_emulator::gb_emulator(std::string rom_filename)
    : m_memory_map(), m_cpu(m_memory_map), m_cycles(0) {
    std::ifstream rom_file (rom_filename, std::ifstream::binary);

    rom_file.seekg(0, rom_file.end);
    m_binsize = std::min(static_cast<int>(rom_file.tellg()), 0x10000);
    rom_file.seekg(0, rom_file.beg);

    gb_test_ram *test_ram = new gb_test_ram();

    rom_file.read(reinterpret_cast<char*>(test_ram->m_mem), m_binsize);

    rom_file.close();

    m_device_list.push_back(static_cast<gb_memory_mapped_device*>(test_ram));
    m_memory_map.add_readable_device(test_ram, 0x0000, 0xFFFF);
    m_memory_map.add_writeable_device(test_ram, 0x0000, 0xFFFF);
}

gb_emulator::~gb_emulator() {
    for (gb_memory_mapped_device* device : m_device_list) {
        delete device;
    }
}

void gb_emulator::tracing(bool enable) {
    m_cpu.tracing(enable);
}

bool gb_emulator::go() {
    while (m_cpu.get_pc() >= 0x0100) {
        m_cycles += m_cpu.step();
    }

    return true;
}
