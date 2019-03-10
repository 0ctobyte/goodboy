#ifndef GB_MEMORY_MAP_H_
#define GB_MEMORY_MAP_H_

#include <cstdint>
#include <vector>

#include "gb_memory_mapped_device.h"

#define GB_MEMORY_MAP_IO_BASE           (0xFF00)
#define GB_MEMORY_MAP_SIZE              (0x10000)

class gb_memory_map {
public:
    gb_memory_map();
    ~gb_memory_map();

    void add_readable_device(gb_memory_mapped_device* device, uint16_t start_addr, size_t size);
    void add_writeable_device(gb_memory_mapped_device* device, uint16_t start_addr, size_t size);
    uint8_t read_byte(uint16_t addr);
    void write_byte(uint16_t addr, uint8_t val);

private:
    typedef std::vector<gb_memory_mapped_device*> gb_device_map_t;

    // Memory is split into LOMEM (0000 - E000) and HIMEM (FE00 - 10000)
    // This way we can split up the granularity of region sizes for each hash table
    // LOMEM hash tables have a granularity of 2KB (i.e. each device must map a minimum of 2KB regions).
    // HIMEM hash tables have a granularity of 1 byte (for IO registers etc.)
    // The idea is that each device maps directly and completely (i.e no holes) into one or more entries in one or both tables
    // Also, we split tables by readable and writeable devices (i.e. ROM vs RAM or RO registers)
    gb_device_map_t m_lomem_readable_devices;
    gb_device_map_t m_lomem_writeable_devices;
    gb_device_map_t m_himem_readable_devices;
    gb_device_map_t m_himem_writeable_devices;

    void _add_device_to_map(gb_device_map_t& device_map, gb_memory_mapped_device* device, uint16_t start_addr, size_t size, size_t bucket_size);
};

#endif // GB_MEMORY_MAP_H_
