#ifndef GB_TEST_RAM_
#define GB_TEST_RAM_

#include <cstdint>
#include <string>

#include "gb_memory_map.h"

class gb_test_ram : public gb_memory_mapped_device {
public:
    uint8_t m_mem[GB_MEMORY_MAP_SIZE];
    std::string m_str;

    gb_test_ram();
    virtual ~gb_test_ram();

    virtual uint8_t read_byte(uint16_t addr);
    virtual void write_byte(uint16_t addr, uint8_t val);
};

#endif // GB_TEST_RAM_
