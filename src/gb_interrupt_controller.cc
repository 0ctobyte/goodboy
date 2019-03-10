#include <stdexcept>
#include <algorithm>

#include "gb_interrupt_controller.h"

#define GB_IFLAGS_ADDR   (0xFF0F)
#define GB_IENABLE_ADDR  (0xFFFF)

gb_interrupt_controller::gb_interrupt_flags::gb_interrupt_flags()
    : gb_memory_mapped_device(GB_IFLAGS_ADDR, 1)
{
}

gb_interrupt_controller::gb_interrupt_flags::~gb_interrupt_flags() {
}

gb_interrupt_controller::gb_interrupt_enable::gb_interrupt_enable()
    : gb_memory_mapped_device(GB_IENABLE_ADDR, 1)
{
}

gb_interrupt_controller::gb_interrupt_enable::~gb_interrupt_enable() {
}

gb_interrupt_controller::gb_interrupt_controller(gb_memory_map& memory_map, gb_cpu& cpu)
    : m_iflags(std::make_shared<gb_interrupt_flags>()),
      m_ienable(std::make_shared<gb_interrupt_enable>()),
      m_cpu(cpu)
{
    // Add the interrupt flags and enable register to the memory map
    gb_address_range_t addr_range = m_iflags->get_address_range();
    memory_map.add_readable_device(m_iflags, std::get<0>(addr_range), std::get<1>(addr_range));

    addr_range = m_ienable->get_address_range();
    memory_map.add_readable_device(m_ienable, std::get<0>(addr_range), std::get<1>(addr_range));
}

gb_interrupt_controller::~gb_interrupt_controller() {
}

void gb_interrupt_controller::add_interrupt_source(const gb_interrupt_source_ptr& interrupt_source) {
    if (interrupt_source == nullptr) throw std::invalid_argument("gb_interrupt_controller::add_interrupt_source - got nullptr");

    // Add interrupt source and then sort the sources by priority (i.e. flag bit) so highest priority source is last
    m_interrupt_sources.push_back(interrupt_source);
    std::sort(m_interrupt_sources.begin(), m_interrupt_sources.end(),
        [](const gb_interrupt_source_ptr& a, const gb_interrupt_source_ptr& b) -> bool {
            return (a->get_flag_mask() > b->get_flag_mask());
    });
}

void gb_interrupt_controller::update(int cycles) {
    gb_interrupt_source* highest_priority_isource = nullptr;

    // For each interrupt source, call it's update routine and then depending on it's return value update the interrupt flags register
    // Keep track of the highest priority interrupt source that last raised an interrupt; this is the interrupt we redirect the CPU to handle
    for (gb_interrupt_source_ptr& isource : m_interrupt_sources) {
        bool interrupt_raised = isource->update(cycles);
        if (interrupt_raised) {
            m_iflags->write_byte(GB_IFLAGS_ADDR, m_iflags->read_byte(GB_IFLAGS_ADDR) | isource->get_flag_mask());
            highest_priority_isource = isource.get();
        }
    }

    // Handle the highest priority interrupt source if an interrupt was raised
    // The CPU will return true/false if it handled the interrupt; it may not if it's interrupt flag is disabled
    // Update the iflags based on if the CPU handled the interrupt or not
    if (highest_priority_isource != nullptr) {
        uint8_t flag_mask = highest_priority_isource->get_flag_mask();
        if ((m_ienable->read_byte(GB_IENABLE_ADDR) & flag_mask) && m_cpu.handle_interrupt(highest_priority_isource->get_jump_address())) {
            m_iflags->write_byte(GB_IFLAGS_ADDR, m_iflags->read_byte(GB_IFLAGS_ADDR) & ~flag_mask);
        }
    }
}
