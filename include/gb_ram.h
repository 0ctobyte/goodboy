#ifndef GB_RAM_
#define GB_RAM_

#include <cstdint>
#include <string>

#include "gb_memory_map.h"

class gb_ram : public gb_memory_mapped_device {
public:
    std::string m_str;

    gb_ram(uint16_t start_addr, size_t size);
    virtual ~gb_ram();

    virtual void write_byte(uint16_t addr, uint8_t val);
};

#endif // GB_RAM_
