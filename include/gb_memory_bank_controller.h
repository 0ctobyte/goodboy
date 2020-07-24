/*
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef GB_MEMORY_BANK_CONTROLLER_
#define GB_MEMORY_BANK_CONTROLLER_

#include "gb_memory_map.h"
#include "gb_rom.h"
#include "gb_ram.h"
#include "gb_rtc.h"

namespace gb_memory_bank_controller {
    gb_memory_mapped_device_ptr make_mbc(gb_memory_manager& memory_manager, gb_memory_map& memory_map, const std::string& rom_filename);
}

class gb_mbc1 : public gb_memory_mapped_device {
public:
    gb_mbc1(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_rom_ptr rom, gb_ram_ptr ram);

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual uint8_t read_byte(uint16_t addr) override;

private:
    gb_memory_map& m_memory_map;
    gb_rom_ptr     m_rom;
    gb_ram_ptr     m_ram;
    bool           m_rom_or_ram_mode;
};

class gb_mbc2 : public gb_memory_mapped_device {
public:
    gb_mbc2(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_rom_ptr rom, gb_ram_ptr ram);

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual uint8_t read_byte(uint16_t addr) override;

private:
    gb_memory_map& m_memory_map;
    gb_rom_ptr     m_rom;
    gb_ram_ptr     m_ram;
};

class gb_mbc3 : public gb_memory_mapped_device {
public:
    gb_mbc3(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_rom_ptr rom, gb_ram_ptr ram, gb_rtc_ptr rtc);

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual uint8_t read_byte(uint16_t addr) override;

private:
    gb_memory_map& m_memory_map;
    gb_rom_ptr     m_rom;
    gb_ram_ptr     m_ram;
    gb_rtc_ptr     m_rtc;
    uint8_t        m_rtc_latch;
};

class gb_mbc5 : public gb_memory_mapped_device {
public:
    gb_mbc5(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_rom_ptr rom, gb_ram_ptr ram);

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual uint8_t read_byte(uint16_t addr) override;

private:
    gb_memory_map& m_memory_map;
    gb_rom_ptr     m_rom;
    gb_ram_ptr     m_ram;
};

#endif // GB_MEMORY_BANK_CONTROLLER_
