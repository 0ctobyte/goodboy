#include <iostream>

#include "gb_memory_map.h"

//uint8_t mem[0x10000];

gb_memory_map::gb_memory_map() {
}

gb_memory_map::~gb_memory_map() {
}

void gb_memory_map::add_readable_device(gb_memory_mapped_device* device, uint16_t start_addr, uint16_t end_addr) {
    _add_device_to_map(m_readable_devices, device, start_addr, end_addr);
}

void gb_memory_map::add_writeable_device(gb_memory_mapped_device* device, uint16_t start_addr, uint16_t end_addr) {
    _add_device_to_map(m_writable_devices, device, start_addr, end_addr);
}

uint8_t gb_memory_map::read_byte(uint16_t addr) {
    uint16_t idx = addr / GB_MEMORY_MAP_BUCKET_SIZE;
    gb_device_list_t& device_list = m_readable_devices[idx];

    for (gb_memory_mapped_device* device : device_list) {
        try {
            uint8_t val = device->read_byte(addr);
            return val;
        } catch (std::out_of_range oor) {}
    }

    std::cout << "Address not implemented: " << addr << std::endl;

    return 0;
}

void gb_memory_map::write_byte(uint16_t addr, uint8_t data) {
    uint16_t idx = addr / GB_MEMORY_MAP_BUCKET_SIZE;
    gb_device_list_t& device_list = m_readable_devices[idx];

    bool written = false;
    for (gb_memory_mapped_device* device : device_list) {
        try {
            device->write_byte(addr, data);
            written = true;
        } catch (std::out_of_range oor) {}
    }

    if (!written) std::cout << "Address not implemented: " << addr << std::endl;
}

void gb_memory_map::_add_device_to_map(gb_device_map_t& device_map, gb_memory_mapped_device* device, uint16_t start_addr, uint16_t end_addr) {
    for (uint64_t i = start_addr; i < end_addr; i += GB_MEMORY_MAP_BUCKET_SIZE) {
        uint16_t idx = static_cast<uint16_t>(i) / GB_MEMORY_MAP_BUCKET_SIZE;
        device_map[idx].push_back(device);
    }
}
