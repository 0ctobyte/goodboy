#ifndef GB_EMULATOR_H_
#define GB_EMULATOR_H_

#include <cstdint>
#include <string>
#include <vector>

#include "gb_memory_map.h"
#include "gb_interrupt_controller.h"
#include "gb_cpu.h"
#include "gb_renderer.h"

class gb_emulator {
friend class gb_debugger;
public:
    gb_emulator();
    ~gb_emulator();

    void load_rom(const std::string& rom_filename);
    void step(int num_cycles);
    void go();

protected:
    gb_renderer              m_renderer;
    gb_memory_manager        m_memory_manager;
    gb_memory_map            m_memory_map;
    gb_cpu                   m_cpu;
    gb_interrupt_controller  m_interrupt_controller;
    uint64_t                 m_cycles;
};

#endif // GB_EMULATOR_H_
