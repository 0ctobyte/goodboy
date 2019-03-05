#ifndef GB_MEMORY_MAP_H_
#define GB_MEMORY_MAP_H_

#include <cstdint>
#include <vector>

#include "gb_memory_mapped_device.h"

#define GB_MEMORY_MAP_IO_BASE (0xFF00)

class gb_memory_map {
public:
    gb_memory_map();
    ~gb_memory_map();

private:
    std::vector<gb_memory_mapped_device> m_memory_mapped_devices;
};

extern uint8_t mem[0x10000];

#endif // GB_MEMORY_MAP_H_
