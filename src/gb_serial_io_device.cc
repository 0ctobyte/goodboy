#include "gb_logger.h"
#include "gb_serial_io_device.h"

#define GB_SERIAL_IO_SB_ADDR (0xFF01)
#define GB_SERIAL_IO_SC_ADDR (0xFF02)

gb_serial_io_device::gb_serial_io_device()
    : gb_memory_mapped_device(GB_SERIAL_IO_SB_ADDR, 2)
{
}

gb_serial_io_device::~gb_serial_io_device() {
}

void gb_serial_io_device::write_byte(uint16_t addr, uint8_t val) {
    // External clock is not supported, do nothing
    if (addr == GB_SERIAL_IO_SC_ADDR && (val & 0x80) && (val & 0x1)) {
        // Append character to the string and clear the transfer start flag to indicate transfer completion
        // TODO: This should model the actual system where transfers take 8*8192 Hz (internal clock) before
        // the start flag is cleared and an interrupt is generated
        // Also since there is no external GameBoy connected, the serial byte register should contain 0xFF
        // after a transfer is complete
        m_str.push_back(static_cast<char>(gb_memory_mapped_device::read_byte(GB_SERIAL_IO_SB_ADDR)));
        gb_memory_mapped_device::write_byte(GB_SERIAL_IO_SC_ADDR, (val & ~0x80));
        gb_memory_mapped_device::write_byte(GB_SERIAL_IO_SB_ADDR, 0xFF);
    } else {
        gb_memory_mapped_device::write_byte(addr, val);
    }

    // If a newline is appended then print it to the terminal
    if (m_str.back() == '\n') {
        GB_LOGGER(GB_LOG_FATAL) << m_str << std::endl;
        m_str.clear();
    }
}
