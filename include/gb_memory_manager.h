/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#ifndef GB_MEMORY_MANAGER_H_
#define GB_MEMORY_MANAGER_H_

#include <cstdint>
#include <vector>

// This is the class that manages all the memory allocation needed by gb_memory_mapped_device's
// The idea is to have all gb_memory_mapped_device's memory be coalesced into one vector instead
// of each gb_memory_mapped_device allocating it's own vectors. This should help with having all // gameboy memory accesses be temporally and spatially localized
class gb_memory_manager {
public:
    gb_memory_manager();
    ~gb_memory_manager();

    // Allocate a range in the vector of size 'size'
    // Effectively this resizes the vector and returns an index into beginning
    // of the newly added portion of the vector
    unsigned long allocate(size_t size);

    // Get a pointer to the beginning of a range of memory starting at addr
    // Useful for memcpy'ing data to parts of the vector (but can be dangerous!!)
    // The returned pointer should not be saved and care must be taken to not overwrite
    // data in other regions or past the end of the vector
    uint8_t* get_mem(unsigned long addr);

    uint8_t read_byte(unsigned long addr);
    void write_byte(unsigned long addr, uint8_t val);

private:
    using gb_mem_t = std::vector<uint8_t>;

    gb_mem_t m_mem;
};

#endif // GB_MEMORY_MANAGER_H_
