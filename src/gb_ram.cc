#include "gb_ram.h"

gb_ram::gb_ram(uint16_t start_addr, size_t size)
    : gb_memory_mapped_device(start_addr, size)
{
}

gb_ram::~gb_ram() {
}
