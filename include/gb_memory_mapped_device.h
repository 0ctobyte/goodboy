#ifndef GB_MEMORY_MAPPED_DEVICE_
#define GB_MEMORY_MAPPED_DEVICE_

#include <cstdint>
#include <vector>
#include <tuple>

using gb_address_range_t = std::tuple<uint16_t,size_t>;

class gb_memory_mapped_device {
public:
    gb_memory_mapped_device(uint16_t start_addr, size_t size);
    virtual ~gb_memory_mapped_device();

    virtual uint8_t* get_mem();
    gb_address_range_t get_address_range() const;
    virtual bool in_range(uint16_t addr) const;
    virtual uint16_t translate(uint16_t addr) const;
    virtual uint8_t read_byte(uint16_t addr);
    virtual void write_byte(uint16_t addr, uint8_t val);

protected:
    using gb_mem_t = std::vector<uint8_t>;

    uint16_t             m_start_addr;
    size_t               m_size;
    gb_mem_t             m_mem;
};

using gb_memory_mapped_device_ptr = std::shared_ptr<gb_memory_mapped_device>;

#endif // GB_MEMORY_MAPPED_DEVICE_
