#ifndef GB_RAM_
#define GB_RAM_

#include <cstdint>
#include <string>

#include "gb_memory_mapped_device.h"

class gb_ram : public gb_memory_mapped_device {
public:
    gb_ram(uint16_t start_addr, size_t size);
    virtual ~gb_ram();
};

#endif // GB_RAM_
