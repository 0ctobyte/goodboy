#ifndef GB_RAM_
#define GB_RAM_

#include <cstdint>
#include <string>

#include "gb_memory_mapped_device.h"

class gb_ram : public gb_memory_mapped_device {
public:
    gb_ram(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size, size_t ram_size);
    virtual ~gb_ram() override;

    virtual unsigned long translate(uint16_t addr) const override;
    unsigned long get_current_bank() const;
    void set_current_bank(unsigned long bank);

private:
    unsigned long m_ram_size;
    unsigned long m_num_banks;
    unsigned long m_cur_bank;
};

using gb_ram_ptr = std::shared_ptr<gb_ram>;

#endif // GB_RAM_
