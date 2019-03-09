#ifndef GB_MEMORY_MAPPED_DEVICE_
#define GB_MEMORY_MAPPED_DEVICE_

#include <cstdint>
#include <vector>

class gb_memory_mapped_device {
public:
    gb_memory_mapped_device(uint16_t start_addr, size_t size);
    virtual ~gb_memory_mapped_device();

    virtual uint8_t* get_mem();
    virtual bool in_range(uint16_t addr);
    virtual uint16_t translate(uint16_t addr);
    virtual uint8_t read_byte(uint16_t addr);
    virtual void write_byte(uint16_t addr, uint8_t val);

protected:
    uint16_t             m_start_addr;
    size_t               m_size;
    uint8_t*             m_mem;
};

#endif // GB_MEMORY_MAPPED_DEVICE_
