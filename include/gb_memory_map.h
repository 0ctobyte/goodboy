#ifndef GB_MEMORY_MAP_H_
#define GB_MEMORY_MAP_H_

#include <cstdint>
#include <array>
#include <vector>

#include "gb_memory_mapped_device.h"

#define GB_MEMORY_MAP_IO_BASE      (0xFF00)
#define GB_MEMORY_MAP_SIZE         (0x10000)
#define GB_MEMORY_MAP_BUCKET_SIZE  (0x1000)
#define GB_MEMORY_MAP_NUM_BUCKETS  (GB_MEMORY_MAP_SIZE/GB_MEMORY_MAP_BUCKET_SIZE)

class gb_memory_map {
public:
    gb_memory_map();
    ~gb_memory_map();

    void add_readable_device(gb_memory_mapped_device* device, uint16_t start_addr, uint16_t end_addr);
    void add_writeable_device(gb_memory_mapped_device* device, uint16_t start_addr, uint16_t end_addr);
    uint8_t read_byte(uint16_t addr);
    void write_byte(uint16_t addr, uint8_t val);

private:
    typedef std::vector<gb_memory_mapped_device*> gb_device_list_t;
    typedef std::array<gb_device_list_t, GB_MEMORY_MAP_NUM_BUCKETS> gb_device_map_t;

    gb_device_map_t m_readable_devices;
    gb_device_map_t m_writable_devices;

    void _add_device_to_map(gb_device_map_t& device_map, gb_memory_mapped_device* device, uint16_t start_addr, uint16_t end_addr);
};

// extern uint8_t mem[GB_MEMORY_MAP_SIZE];

#endif // GB_MEMORY_MAP_H_
