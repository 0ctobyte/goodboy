#ifndef GB_INTERRUPT_CONTROLLER_H_
#define GB_INTERRUPT_CONTROLLER_H_

#include <cstdint>

#include "gb_memory_map.h"
#include "gb_interrupt_source.h"
#include "gb_cpu.h"

class gb_interrupt_controller {
public:
    gb_interrupt_controller(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_cpu& cpu);
    ~gb_interrupt_controller();

    void add_interrupt_source(const gb_interrupt_source_ptr& interrupt_source);
    void update(int cycles);

private:
    // The gb_interrupt_flags and gb_interrupt_enable objects contain the memory mapped
    // flags and enable registers
    gb_memory_mapped_device_ptr          m_iflags;
    gb_memory_mapped_device_ptr          m_ienable;

    // The interrupt controller needs access to the CPU to send it interrupt sources to handle
    gb_cpu&                              m_cpu;

    // List of interrupt sources to update every iteration
    std::vector<gb_interrupt_source_ptr> m_interrupt_sources;
};

#endif // GB_INTERRUPT_CONTROLLER_H_
