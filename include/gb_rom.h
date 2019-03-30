#ifndef GB_ROM_
#define GB_ROM_

#include <cstdint>
#include <string>

#include "gb_memory_mapped_device.h"

class gb_rom : public gb_memory_mapped_device {
public:
    gb_rom(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size, size_t rom_size);
    virtual ~gb_rom() override;

    virtual unsigned long translate(uint16_t addr) const override;
    virtual void write_byte(uint16_t addr, uint8_t val) override;
    unsigned long get_current_bank() const;
    void set_current_bank(unsigned long bank);

private:
    unsigned long m_num_banks;
    unsigned long m_cur_bank;
};

using gb_rom_ptr = std::shared_ptr<gb_rom>;

#endif // GB_ROM_
