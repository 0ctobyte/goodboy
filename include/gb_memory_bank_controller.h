#ifndef GB_MEMORY_BANK_CONTROLLER_
#define GB_MEMORY_BANK_CONTROLLER_

#include "gb_memory_map.h"
#include "gb_rom.h"
#include "gb_ram.h"

namespace gb_memory_bank_controller {
    gb_memory_mapped_device_ptr make_mbc(gb_memory_manager& memory_manager, gb_memory_map& memory_map, const std::string& rom_filename);
}

class gb_mbc1 : public gb_memory_mapped_device {
public:
    gb_mbc1(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_rom_ptr rom, gb_ram_ptr ram);
    virtual ~gb_mbc1() override;

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual uint8_t read_byte(uint16_t addr) override;

private:
    gb_memory_map& m_memory_map;
    gb_rom_ptr     m_rom;
    gb_ram_ptr     m_ram;
    bool           m_rom_or_ram_mode;

    void mbc1_enable_ram(uint8_t data);
    void mbc1_set_rom_bank_lo(uint8_t data);
    void mbc1_set_rom_bank_hi(uint8_t data);
    void mbc1_set_ram_bank(uint8_t data);
    void mbc1_set_rom_or_ram_bank(uint8_t data);
    void mbc1_set_rom_or_ram_mode(uint8_t data);
};

#endif // GB_MEMORY_BANK_CONTROLLER_
