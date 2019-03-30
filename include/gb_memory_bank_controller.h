#ifndef GB_MEMORY_BANK_CONTROLLER_
#define GB_MEMORY_BANK_CONTROLLER_

#include <unordered_map>
#include <functional>

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
    enum gb_mbc_type_t {
        GB_MBC_NONE,
        GB_MBC_TYPE1,
        GB_MBC_TYPE2,
        GB_MBC_TYPE3,
        GB_MBC_TYPE5,
        GB_MBC_UNKNOWN
    };

    struct gb_cartridge_attributes_t {
        std::string   mbc_str;
        gb_mbc_type_t mbc_type;
        bool          has_ram;
        bool          has_battery;
        bool          has_rtc;
    };

    using gb_cartridge_attributes_map_t = std::unordered_map<uint8_t, gb_cartridge_attributes_t>;
    using gb_mbc_action_map_t = std::unordered_map<uint8_t, std::function<void(uint8_t)>>;

    static gb_cartridge_attributes_map_t m_cartridge_attr_map;

    gb_memory_map&            m_memory_map;
    gb_cartridge_attributes_t m_cartridge_attr;
    gb_mbc_action_map_t       m_mbc_actions;
    bool                      m_rom_or_ram_mode;
    gb_rom_ptr                m_rom0;
    gb_rom_ptr                m_rom1;
    gb_ram_ptr                m_ram;

    // MBC1 actions
    void mbc1_enable_ram(uint8_t data);
    void mbc1_set_rom_bank_lo(uint8_t data);
    void mbc1_set_rom_bank_hi(uint8_t data);
    void mbc1_set_ram_bank(uint8_t data);
    void mbc1_set_rom_or_ram_bank(uint8_t data);
    void mbc1_set_rom_or_ram_mode(uint8_t data);
};

using gb_memory_bank_controller_ptr = std::shared_ptr<gb_memory_bank_controller>;

#endif // GB_MEMORY_BANK_CONTROLLER_
