#include <algorithm>

#include "gb_logger.h"
#include "gb_serial_io.h"

#define GB_SERIAL_IO_SB_ADDR              (0xFF01)
#define GB_SERIAL_IO_SC_ADDR              (0xFF02)
#define GB_SERIAL_IO_JUMP_ADDR            (0x58)
#define GB_SERIAL_IO_FLAG_BIT             (0x3)

#define CLOCK_SPEED                       (4194304)
#define GB_SERIAL_IO_INTERNAL_CLOCK_FREQ  (8192)
#define GB_SERIAL_IO_CYCLES_TO_IRQ        ((CLOCK_SPEED)/((GB_SERIAL_IO_INTERNAL_CLOCK_FREQ)/8))

gb_serial_io::gb_serial_io(gb_memory_manager& memory_manager)
    : gb_memory_mapped_device(memory_manager, GB_SERIAL_IO_SB_ADDR, 2),
      gb_interrupt_source(GB_SERIAL_IO_JUMP_ADDR, GB_SERIAL_IO_FLAG_BIT),
      m_irq_counter(0)
{
}

gb_serial_io::~gb_serial_io() {
}

void gb_serial_io::write_byte(uint16_t addr, uint8_t val) {
    if (addr == GB_SERIAL_IO_SC_ADDR) {
        // Unused bits in the SC register are read-as-one
        // SC
        // 7   - transfer start flag
        // 6:1 - reserved RAO
        // 0   - shift clock select
        val |= 0x7e;

        // External clock is not supported, do nothing
        if ((val & 0x80) && (val & 0x1)) {
            // Append character to the string and set the IRQ down counter
            // = cpu_freq / (serial_io_freq/8) = # of CPU clock cycles to wait before asserting an interrupt (divide by 8 because it needs to shift in/out 8 bits)
            m_str.push_back(static_cast<char>(gb_memory_mapped_device::read_byte(GB_SERIAL_IO_SB_ADDR)));
            m_irq_counter = GB_SERIAL_IO_CYCLES_TO_IRQ;
        }
    }

    gb_memory_mapped_device::write_byte(addr, val);
}

bool gb_serial_io::update(int cycles) {
    if (m_irq_counter == 0) return false;

    // This should model the actual system where transfers take 8*8192 Hz (internal clock) before
    // the start flag is cleared and an interrupt is generated
    // Decrement counter until zero; which is when we return true to indicate an interrupt
    // Also since there is no external GameBoy connected, the serial byte register should contain 0xFF
    // after a transfer is complete
    m_irq_counter = std::max(0, m_irq_counter-cycles);

    if (m_irq_counter == 0) {
        gb_memory_mapped_device::write_byte(GB_SERIAL_IO_SC_ADDR, (gb_memory_mapped_device::read_byte(GB_SERIAL_IO_SC_ADDR)  & ~0x80));
        gb_memory_mapped_device::write_byte(GB_SERIAL_IO_SB_ADDR, 0xFF);

        // If a newline was last appended then print it to the terminal
        if (m_str.back() == '\n') {
            GB_LOGGER(GB_LOG_FATAL) << m_str << std::endl;
            m_str.clear();
        }

        return true;
    }

    return false;
}
