#include <algorithm>
#include <stdexcept>

#include "gb_logger.h"
#include "gb_memory_map.h"

#define GB_MEMORY_MAP_WKRAM_START       (0xC000)
#define GB_MEMORY_MAP_EORAM_START       (0xE000)
#define GB_MEMORY_MAP_LOMEM_SIZE        (GB_MEMORY_MAP_EORAM_START)
#define GB_MEMORY_MAP_LOMEM_BUCKET_SIZE (0x800)
#define GB_MEMORY_MAP_LOMEM_NUM_BUCKETS (GB_MEMORY_MAP_LOMEM_SIZE/GB_MEMORY_MAP_LOMEM_BUCKET_SIZE)
#define GB_MEMORY_MAP_HIMEM_START       (0xFE00)
#define GB_MEMORY_MAP_HIMEM_SIZE        (0x200)
#define GB_MEMORY_MAP_HIMEM_BUCKET_SIZE (0x1)
#define GB_MEMORY_MAP_HIMEM_NUM_BUCKETS (GB_MEMORY_MAP_HIMEM_SIZE/GB_MEMORY_MAP_HIMEM_BUCKET_SIZE)

gb_memory_map::gb_memory_map()
    : m_lomem_readable_devices(GB_MEMORY_MAP_LOMEM_NUM_BUCKETS, nullptr),
      m_lomem_writeable_devices(GB_MEMORY_MAP_LOMEM_NUM_BUCKETS, nullptr),
      m_himem_readable_devices(GB_MEMORY_MAP_HIMEM_NUM_BUCKETS, nullptr),
      m_himem_writeable_devices(GB_MEMORY_MAP_HIMEM_NUM_BUCKETS, nullptr)
{
}

gb_memory_map::~gb_memory_map() {
}

void gb_memory_map::_add_device_to_map(gb_device_map_t& device_map, gb_memory_mapped_device_ptr device, uint16_t start_addr, size_t end_addr, size_t bucket_size) {
    if (device == nullptr) throw std::invalid_argument("gb_memory_map::_add_device_to_map - got nullptr");

    for (uint64_t i = start_addr; i < end_addr; i += bucket_size) {
        uint16_t idx = static_cast<uint16_t>(i) / bucket_size;
        device_map.at(idx) = device;
    }
}

void gb_memory_map::add_readable_device(gb_memory_mapped_device_ptr device, uint16_t start_addr, size_t size) {
    size_t end_addr = start_addr + size;

    // Add device to either the LOMEM or HIMEM lists or both depending on the address range
    if (start_addr < GB_MEMORY_MAP_EORAM_START) {
        _add_device_to_map(m_lomem_readable_devices, device, start_addr, std::min<size_t>(end_addr, GB_MEMORY_MAP_EORAM_START-1), GB_MEMORY_MAP_LOMEM_BUCKET_SIZE);
        // Add device to both LOMEM and HIMEM if the span crosses both regions
        if (end_addr >= GB_MEMORY_MAP_HIMEM_START) {
            _add_device_to_map(m_himem_readable_devices, device, 0, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
        }
    } else if (start_addr >= GB_MEMORY_MAP_HIMEM_START) {
        _add_device_to_map(m_himem_readable_devices, device, start_addr-GB_MEMORY_MAP_HIMEM_START, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
    }
}

void gb_memory_map::add_writeable_device(gb_memory_mapped_device_ptr device, uint16_t start_addr, size_t size) {
    size_t end_addr = start_addr + size;

    // Add device to either the LOMEM or HIMEM lists or both depending on the address range
    if (start_addr < GB_MEMORY_MAP_EORAM_START) {
        _add_device_to_map(m_lomem_writeable_devices, device, start_addr, std::min<size_t>(end_addr, GB_MEMORY_MAP_EORAM_START-1), GB_MEMORY_MAP_LOMEM_BUCKET_SIZE);
        // Add device to both LOMEM and HIMEM if the span crosses both regions
        if (end_addr >= GB_MEMORY_MAP_HIMEM_START) {
            _add_device_to_map(m_himem_writeable_devices, device, 0, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
        }
    } else if (end_addr >= GB_MEMORY_MAP_HIMEM_START) {
        _add_device_to_map(m_himem_writeable_devices, device, start_addr-GB_MEMORY_MAP_HIMEM_START, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
    }
}

uint8_t gb_memory_map::read_byte(uint16_t addr) {
    uint16_t naddr = addr;
    gb_memory_mapped_device* device = nullptr;

    // Determine if address is either in LOMEM, in the Echo RAM space, or in HIMEM
    if (addr < GB_MEMORY_MAP_LOMEM_SIZE) {
        device = m_lomem_readable_devices.at(naddr / GB_MEMORY_MAP_LOMEM_BUCKET_SIZE).get();
    } else if (addr < GB_MEMORY_MAP_HIMEM_START) {
        // Echo RAM
        naddr = (addr - GB_MEMORY_MAP_EORAM_START) + GB_MEMORY_MAP_WKRAM_START;
        device = m_lomem_readable_devices.at(naddr / GB_MEMORY_MAP_LOMEM_BUCKET_SIZE).get();
    } else {
        device = m_himem_readable_devices.at((naddr - GB_MEMORY_MAP_HIMEM_START) / GB_MEMORY_MAP_HIMEM_BUCKET_SIZE).get();
    }

    if (device == nullptr) {
        GB_LOGGER(GB_LOG_WARN) << "read_byte: Address not implemented: " << std::hex << addr << std::endl;
        return 0;
    }

    uint8_t data = device->read_byte(naddr);

    return data;
}

void gb_memory_map::write_byte(uint16_t addr, uint8_t data) {
    uint16_t naddr = addr;
    gb_memory_mapped_device* device = nullptr;

    // Determine if address is either in LOMEM, in the Echo RAM space, or in HIMEM
    if (addr < GB_MEMORY_MAP_LOMEM_SIZE) {
        device = m_lomem_writeable_devices.at(naddr / GB_MEMORY_MAP_LOMEM_BUCKET_SIZE).get();
    } else if (addr < GB_MEMORY_MAP_HIMEM_START) {
        // Echo RAM
        naddr = (addr - GB_MEMORY_MAP_EORAM_START) + GB_MEMORY_MAP_WKRAM_START;
        device = m_lomem_writeable_devices.at(naddr / GB_MEMORY_MAP_LOMEM_BUCKET_SIZE).get();
    } else {
        device = m_himem_writeable_devices.at((naddr - GB_MEMORY_MAP_HIMEM_START) / GB_MEMORY_MAP_HIMEM_BUCKET_SIZE).get();
    }

    // Ensure we actually have a device that can handle this write request
    if (device == nullptr) {
        GB_LOGGER(GB_LOG_WARN) << "write_byte: Address not implemented: " << std::hex << addr << std::endl;
    } else {
        device->write_byte(naddr, data);
    }
}
