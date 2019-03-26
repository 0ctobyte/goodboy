#ifndef GB_MEMORY_BANK_CONTROLLER_
#define GB_MEMORY_BANK_CONTROLLER_

#include "gb_memory_map.h"
#include "gb_rom.h"
#include "gb_ram.h"

class gb_memory_bank_controller : public gb_memory_mapped_device {
public:
    gb_memory_bank_controller(gb_memory_manager& memory_manager, gb_memory_map& memory_map);
    virtual ~gb_memory_bank_controller() override;

    virtual void load_rom(const std::string& rom_filename);
    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual uint8_t read_byte(uint16_t addr) override;

private:
    bool           m_has_ram;
    bool           m_has_battery;
    bool           m_has_rtc;
    //bool           m_ram_enable;
    //bool           m_ram_rom_mode;
    //unsigned long  m_cur_rom_bank;
    //unsigned long  m_cur_ram_bank;
    gb_memory_map& m_memory_map;
    gb_rom_ptr     m_rom0;
    gb_rom_ptr     m_rom1;
    gb_ram_ptr     m_ram;
};

using gb_memory_bank_controller_ptr = std::shared_ptr<gb_memory_bank_controller>;

#endif // GB_MEMORY_BANK_CONTROLLER_
