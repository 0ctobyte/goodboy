#ifndef GB_MEMORY_MAPPED_DEVICE_
#define GB_MEMORY_MAPPED_DEVICE_

#include <cstdint>

class gb_memory_mapped_device {
public:
    gb_memory_mapped_device();
    virtual ~gb_memory_mapped_device();

    virtual uint8_t read_byte(uint16_t addr);
    virtual void write_byte(uint16_t addr, uint8_t val);
};

#endif // GB_MEMORY_MAPPED_DEVICE_
