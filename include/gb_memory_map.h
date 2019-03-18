#ifndef GB_MEMORY_MAP_H_
#define GB_MEMORY_MAP_H_

#include <cstdint>
#include <array>

#include "gb_memory_mapped_device.h"

#define GB_MEMORY_MAP_IO_BASE           (0xFF00)
#define GB_MEMORY_MAP_SIZE              (0x10000)

#define GB_MEMORY_MAP_WKRAM_START       (0xC000)
#define GB_MEMORY_MAP_EORAM_START       (0xE000)
#define GB_MEMORY_MAP_LOMEM_SIZE        (GB_MEMORY_MAP_EORAM_START)
#define GB_MEMORY_MAP_LOMEM_BUCKET_SIZE (0x800)
#define GB_MEMORY_MAP_LOMEM_NUM_BUCKETS (GB_MEMORY_MAP_LOMEM_SIZE/GB_MEMORY_MAP_LOMEM_BUCKET_SIZE)
#define GB_MEMORY_MAP_HIMEM_START       (0xFE00)
#define GB_MEMORY_MAP_HIMEM_SIZE        (0x200)
#define GB_MEMORY_MAP_HIMEM_BUCKET_SIZE (0x1)
#define GB_MEMORY_MAP_HIMEM_NUM_BUCKETS (GB_MEMORY_MAP_HIMEM_SIZE/GB_MEMORY_MAP_HIMEM_BUCKET_SIZE)

class gb_memory_map {
public:
    gb_memory_map();
    ~gb_memory_map();

    void add_readable_device(const gb_memory_mapped_device_ptr device, uint16_t start_addr, size_t size);
    void add_writeable_device(const gb_memory_mapped_device_ptr device, uint16_t start_addr, size_t size);
    void remove_readable_device(uint16_t start_addr, size_t size);
    void remove_writeable_device(uint16_t start_addr, size_t size);
    gb_memory_mapped_device_ptr get_readable_device(uint16_t addr);
    gb_memory_mapped_device_ptr get_writeable_device(uint16_t addr);
    uint8_t read_byte(uint16_t addr);
    void write_byte(uint16_t addr, uint8_t val);

private:
    template <size_t S>
    using gb_device_map_t     = std::array<gb_memory_mapped_device_ptr, S>;
    using gb_device_address_t = std::tuple<gb_memory_mapped_device_ptr, uint16_t>;

    // Memory is split into LOMEM (0000 - E000) and HIMEM (FE00 - 10000)
    // This way we can split up the granularity of region sizes for each hash table
    // LOMEM hash tables have a granularity of 2KB (i.e. each device must map a minimum of 2KB regions).
    // HIMEM hash tables have a granularity of 1 byte (for IO registers etc.)
    // The idea is that each device maps directly and completely (i.e no holes) into one or more entries in one or both tables
    // Also, we split tables by readable and writeable devices (i.e. ROM vs RAM or RO registers)
    gb_device_map_t<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS> m_lomem_readable_devices;
    gb_device_map_t<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS> m_lomem_writeable_devices;
    gb_device_map_t<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS> m_himem_readable_devices;
    gb_device_map_t<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS> m_himem_writeable_devices;

    template <size_t S>
    void _add_device_to_map(gb_device_map_t<S>& device_map, const gb_memory_mapped_device_ptr& device, uint16_t start_addr, size_t size, size_t bucket_size);
    template <size_t S>
    void _remove_device_from_map(gb_device_map_t<S>& device_map, uint16_t start_addr, size_t size, size_t bucket_size);
    template <size_t S1, size_t S2>
    gb_device_address_t _get_device_from_map(gb_device_map_t<S1>& lomem_device_map, gb_device_map_t<S2>& himem_device_map, uint16_t addr);
};

#endif // GB_MEMORY_MAP_H_
