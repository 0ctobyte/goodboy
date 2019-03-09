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

    gb_device_map_t m_lomem_readable_devices;
    gb_device_map_t m_lomem_writeable_devices;
    gb_device_map_t m_himem_readable_devices;
    gb_device_map_t m_himem_writeable_devices;

    void _add_device_to_map(gb_device_map_t& device_map, gb_memory_mapped_device* device, uint16_t start_addr, size_t size, size_t bucket_size);
};

#endif // GB_MEMORY_MAP_H_
