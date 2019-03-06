#include <stdexcept>

#include "gb_memory_mapped_device.h"

gb_memory_mapped_device::gb_memory_mapped_device() {
}

gb_memory_mapped_device::~gb_memory_mapped_device() {
}

uint8_t gb_memory_mapped_device::read_byte(uint16_t addr) {
    throw std::out_of_range("gb_memory_mapped_device::read_byte");
}

void gb_memory_mapped_device::write_byte(uint16_t addr, uint8_t val) {
    throw std::out_of_range("gb_memory_mapped_device::write_byte");
}
