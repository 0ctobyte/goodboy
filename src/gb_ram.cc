#include "gb_ram.h"

gb_ram::gb_ram(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size)
    : gb_memory_mapped_device(memory_manager, start_addr, size)
{
}

gb_ram::~gb_ram() {
}
