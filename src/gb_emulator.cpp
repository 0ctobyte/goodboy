#include <fstream>

#include "gb_emulator.h"
#include "gb_memory.h"

gb_emulator::gb_emulator(std::string rom_filename)
    : m_cpu(), m_cycles(0) {
    std::ifstream rom_file (rom_filename, std::ifstream::binary);

    rom_file.seekg(0, rom_file.end);
    m_binsize = std::min(static_cast<int>(rom_file.tellg()), 0x10000);
    rom_file.seekg(0, rom_file.beg);

    rom_file.read(reinterpret_cast<char*>(mem), m_binsize);

    rom_file.close();
}

gb_emulator::~gb_emulator() {
}

bool gb_emulator::go() {
    while (m_cpu.get_pc() < m_binsize) {
        m_cycles += m_cpu.step();
    }

    return true;
}
