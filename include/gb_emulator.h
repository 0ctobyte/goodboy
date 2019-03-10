#ifndef GB_EMULATOR_H_
#define GB_EMULATOR_H_

#include <cstdint>
#include <string>
#include <vector>

#include "gb_memory_map.h"
#include "gb_interrupt_controller.h"
#include "gb_cpu.h"

class gb_emulator {
public:
    gb_emulator();

    virtual ~gb_emulator();

    virtual void go();
    virtual bool load_rom(std::string rom_filename);

protected:
    typedef std::vector<gb_memory_mapped_device*> gb_device_list;

    gb_memory_map            m_memory_map;
    gb_cpu                   m_cpu;
    gb_interrupt_controller  m_interrupt_controller;
    gb_device_list           m_device_list;
    uint64_t                 m_cycles;
};

#endif // GB_EMULATOR_H_
