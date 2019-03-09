#ifndef GB_ROM_
#define GB_ROM_

#include <cstdint>
#include <string>

#include "gb_memory_mapped_device.h"

class gb_rom : public gb_memory_mapped_device {
public:
    gb_rom(uint16_t start_addr, size_t size);
    virtual ~gb_rom();

    virtual void write_byte(uint16_t addr, uint8_t val);
};

#endif // GB_ROM_
