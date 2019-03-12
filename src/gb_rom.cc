#include "gb_logger.h"
#include "gb_rom.h"

gb_rom::gb_rom(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size)
    : gb_memory_mapped_device(memory_manager, start_addr, size)
{
}

gb_rom::~gb_rom() {
}

void gb_rom::write_byte(uint16_t addr, uint8_t val) {
    GB_LOGGER(GB_LOG_WARN) << "gb_rom::write_byte - Attempting to write to read-only memory: " << std::hex << addr << " : " << std::hex << static_cast<uint16_t>(val) << std::endl;
}
