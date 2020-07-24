/*
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>
#include <cassert>

#include "gb_logger.h"
#include "gb_memory_map.h"

gb_memory_map::gb_memory_map()
    : m_lomem_readable_devices({}), m_lomem_writeable_devices({}), m_himem_readable_devices({}), m_himem_writeable_devices({})
{
}

gb_memory_map::~gb_memory_map() {
}

template <size_t S>
void gb_memory_map::_add_device_to_map(gb_device_map_t<S>& device_map, const gb_memory_mapped_device_ptr& device, uint16_t start_addr, size_t end_addr, size_t bucket_size) {
    assert(device != NULL);
    for (uint64_t i = start_addr; i < end_addr; i += bucket_size) {
        uint16_t idx = static_cast<uint16_t>(i) / bucket_size;
        device_map.at(idx) = device;
    }
}

template <size_t S>
void gb_memory_map::_remove_device_from_map(gb_device_map_t<S>& device_map, uint16_t start_addr, size_t end_addr, size_t bucket_size) {
    for (uint64_t i = start_addr; i < end_addr; i += bucket_size) {
        uint16_t idx = static_cast<uint16_t>(i) / bucket_size;
        device_map.at(idx) = nullptr;
    }
}

template <size_t S1, size_t S2>
gb_memory_map::gb_device_address_t gb_memory_map::_get_device_from_map(gb_device_map_t<S1>& lomem_device_map, gb_device_map_t<S2>& himem_device_map, uint16_t addr) {
    uint16_t naddr = addr;
    gb_memory_mapped_device_ptr device;

    // Determine if address is either in LOMEM, in the Echo RAM space, or in HIMEM
    if (addr < GB_MEMORY_MAP_LOMEM_SIZE) {
        device = lomem_device_map.at(naddr / GB_MEMORY_MAP_LOMEM_BUCKET_SIZE);
    } else if (addr < GB_MEMORY_MAP_HIMEM_START) {
        // Echo RAM
        naddr = (addr - GB_MEMORY_MAP_EORAM_START) + GB_MEMORY_MAP_WKRAM_START;
        device = lomem_device_map.at(naddr / GB_MEMORY_MAP_LOMEM_BUCKET_SIZE);
    } else {
        device = himem_device_map.at((naddr - GB_MEMORY_MAP_HIMEM_START) / GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
    }

    return std::make_tuple(device, naddr);
}

void gb_memory_map::add_readable_device(const gb_memory_mapped_device_ptr device, uint16_t start_addr, size_t size) {
    size_t end_addr = start_addr + size;

    // Add device to either the LOMEM or HIMEM lists or both depending on the address range
    if (start_addr < GB_MEMORY_MAP_EORAM_START) {
        _add_device_to_map<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS>(m_lomem_readable_devices, device, start_addr, std::min<size_t>(end_addr, GB_MEMORY_MAP_EORAM_START-1), GB_MEMORY_MAP_LOMEM_BUCKET_SIZE);
        // Add device to both LOMEM and HIMEM if the span crosses both regions
        if (end_addr >= GB_MEMORY_MAP_HIMEM_START) {
            _add_device_to_map<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_himem_readable_devices, device, 0, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
        }
    } else if (start_addr >= GB_MEMORY_MAP_HIMEM_START) {
        _add_device_to_map<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_himem_readable_devices, device, start_addr-GB_MEMORY_MAP_HIMEM_START, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
    }
}

void gb_memory_map::remove_readable_device(uint16_t start_addr, size_t size) {
    size_t end_addr = start_addr + size;

    // Remove device from either the LOMEM or HIMEM lists or both depending on the address range
    if (start_addr < GB_MEMORY_MAP_EORAM_START) {
        _remove_device_from_map<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS>(m_lomem_readable_devices, start_addr, std::min<size_t>(end_addr, GB_MEMORY_MAP_EORAM_START-1), GB_MEMORY_MAP_LOMEM_BUCKET_SIZE);
        // Add device to both LOMEM and HIMEM if the span crosses both regions
        if (end_addr >= GB_MEMORY_MAP_HIMEM_START) {
            _remove_device_from_map<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_himem_readable_devices, 0, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
        }
    } else if (start_addr >= GB_MEMORY_MAP_HIMEM_START) {
        _remove_device_from_map<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_himem_readable_devices, start_addr-GB_MEMORY_MAP_HIMEM_START, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
    }
}

void gb_memory_map::add_writeable_device(const gb_memory_mapped_device_ptr device, uint16_t start_addr, size_t size) {
    size_t end_addr = start_addr + size;

    // Add device to either the LOMEM or HIMEM lists or both depending on the address range
    if (start_addr < GB_MEMORY_MAP_EORAM_START) {
        _add_device_to_map<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS>(m_lomem_writeable_devices, device, start_addr, std::min<size_t>(end_addr, GB_MEMORY_MAP_EORAM_START-1), GB_MEMORY_MAP_LOMEM_BUCKET_SIZE);
        // Add device to both LOMEM and HIMEM if the span crosses both regions
        if (end_addr >= GB_MEMORY_MAP_HIMEM_START) {
            _add_device_to_map<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_himem_writeable_devices, device, 0, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
        }
    } else if (end_addr >= GB_MEMORY_MAP_HIMEM_START) {
        _add_device_to_map<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_himem_writeable_devices, device, start_addr-GB_MEMORY_MAP_HIMEM_START, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
    }
}

void gb_memory_map::remove_writeable_device(uint16_t start_addr, size_t size) {
    size_t end_addr = start_addr + size;

    // Remove device from either the LOMEM or HIMEM lists or both depending on the address range
    if (start_addr < GB_MEMORY_MAP_EORAM_START) {
        _remove_device_from_map<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS>(m_lomem_writeable_devices, start_addr, std::min<size_t>(end_addr, GB_MEMORY_MAP_EORAM_START-1), GB_MEMORY_MAP_LOMEM_BUCKET_SIZE);
        // Add device to both LOMEM and HIMEM if the span crosses both regions
        if (end_addr >= GB_MEMORY_MAP_HIMEM_START) {
            _remove_device_from_map<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_himem_writeable_devices, 0, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
        }
    } else if (start_addr >= GB_MEMORY_MAP_HIMEM_START) {
        _remove_device_from_map<GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_himem_writeable_devices, start_addr-GB_MEMORY_MAP_HIMEM_START, end_addr-GB_MEMORY_MAP_HIMEM_START, GB_MEMORY_MAP_HIMEM_BUCKET_SIZE);
    }
}

gb_memory_mapped_device_ptr gb_memory_map::get_readable_device(uint16_t addr) {
    gb_device_address_t dev_addr = _get_device_from_map<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS, GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_lomem_readable_devices, m_himem_readable_devices, addr);

    return std::get<0>(dev_addr);
}

gb_memory_mapped_device_ptr gb_memory_map::get_writeable_device(uint16_t addr) {
    gb_device_address_t dev_addr = _get_device_from_map<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS, GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_lomem_writeable_devices, m_himem_writeable_devices, addr);

    return std::get<0>(dev_addr);
}

uint8_t gb_memory_map::read_byte(uint16_t addr) {
    // Get the device and possibly translated address
    gb_device_address_t dev_addr = _get_device_from_map<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS, GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_lomem_readable_devices, m_himem_readable_devices, addr);

    gb_memory_mapped_device_ptr device = std::get<0>(dev_addr);
    uint16_t naddr = std::get<1>(dev_addr);

    if (device == nullptr) {
        GB_LOGGER(GB_LOG_WARN) << "read_byte: Address not implemented: " << std::hex << addr << std::endl;
        return 0xff;
    }

    uint8_t data = device->read_byte(naddr);

    return data;
}

void gb_memory_map::write_byte(uint16_t addr, uint8_t data) {
    // Get the device and possibly translated address
    gb_device_address_t dev_addr = _get_device_from_map<GB_MEMORY_MAP_LOMEM_NUM_BUCKETS, GB_MEMORY_MAP_HIMEM_NUM_BUCKETS>(m_lomem_writeable_devices, m_himem_writeable_devices, addr);

    gb_memory_mapped_device_ptr device = std::get<0>(dev_addr);
    uint16_t naddr = std::get<1>(dev_addr);

    // Ensure we actually have a device that can handle this write request
    if (device == nullptr) {
        GB_LOGGER(GB_LOG_WARN) << "write_byte: Address not implemented: " << std::hex << addr << " -- " << std::hex << static_cast<uint16_t>(data) << std::endl;
    } else {
        device->write_byte(naddr, data);
    }
}
