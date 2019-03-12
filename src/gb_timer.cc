#include <algorithm>
#include <array>

#include "gb_timer.h"

#define GB_TIMER_DIV_ADDR       (0xFF04)
#define GB_TIMER_TIMA_ADDR      (0xFF05)
#define GB_TIMER_TMA_ADDR       (0xFF06)
#define GB_TIMER_TAC_ADDR       (0xFF07)

#define GB_TIMER_JUMP_ADDR      (0x50)
#define GB_TIMER_FLAG_BIT       (0x2)

// # of CPU cycles per tick = cpu_freq / timer_clk_freq
#define CLOCK_SPEED             (4194304)
#define GB_TIMER_COUNTER_4096   ((CLOCK_SPEED)/4096)
#define GB_TIMER_COUNTER_262144 ((CLOCK_SPEED)/262144)
#define GB_TIMER_COUNTER_65536  ((CLOCK_SPEED)/65536)
#define GB_TIMER_COUNTER_16384  ((CLOCK_SPEED)/16384)

#define TIMER_CLK_SELECT_TBL_INIT \
{{\
   GB_TIMER_COUNTER_4096,\
   GB_TIMER_COUNTER_262144,\
   GB_TIMER_COUNTER_65536,\
   GB_TIMER_COUNTER_16384\
}}

gb_timer::gb_timer(gb_memory_manager& memory_manager)
    : gb_memory_mapped_device(memory_manager, GB_TIMER_DIV_ADDR, 4),
      gb_interrupt_source(GB_TIMER_JUMP_ADDR, GB_TIMER_FLAG_BIT),
      m_timer_clk_select_tbl(TIMER_CLK_SELECT_TBL_INIT), m_div_counter(0), m_timer_counter(0), m_timer_start(false), m_timer_clk_select(0)
{
}

gb_timer::~gb_timer() {
}

void gb_timer::write_byte(uint16_t addr, uint8_t val) {
    if (addr == GB_TIMER_TAC_ADDR) {
        // val[2] == start timer
        // val[1:0] == clock select: 00 - 4096 Hz, 01 - 262144 Hz, 10 - 65536 Hz, 11 - 16384 Hz
        m_timer_start = (val & 0x4);
        m_timer_clk_select = val & 0x3;
        m_timer_counter = m_timer_clk_select_tbl.at(m_timer_clk_select);
    }

    gb_memory_mapped_device::write_byte(addr, val);
}

bool gb_timer::update(int cycles) {
    // Increment the DIV register when the div counter <= 0
    m_div_counter = std::max(0, m_div_counter - cycles);
    if (m_div_counter == 0) gb_memory_mapped_device::write_byte(GB_TIMER_DIV_ADDR, gb_memory_mapped_device::read_byte(GB_TIMER_DIV_ADDR) + 1);

    // Don't generate interrupts if the timer is stopped
    if (!m_timer_start) return false;

    // Increment the TIMA (timer counter) register when the timer counter <= 0
    m_timer_counter = std::max(0, m_timer_counter - cycles);
    uint8_t tima = gb_memory_mapped_device::read_byte(GB_TIMER_TIMA_ADDR) + 1;
    if (m_timer_counter == 0) {
        gb_memory_mapped_device::write_byte(GB_TIMER_TIMA_ADDR, tima);
        m_timer_counter = m_timer_clk_select_tbl.at(m_timer_clk_select);
    }

    // Check if the TIMA register overflowed, if so raise an interrupt and reload the TIMA register with the value in the TMA register
    if (tima == 0) {
        gb_memory_mapped_device::write_byte(GB_TIMER_TIMA_ADDR, gb_memory_mapped_device::read_byte(GB_TIMER_TMA_ADDR));
        return true;
    }

    return false;
}
