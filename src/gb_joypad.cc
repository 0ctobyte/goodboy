#include "gb_joypad.h"

#define GB_JOYPAD_P1_ADDR   (0xFF00)

#define GB_JOYPAD_JUMP_ADDR (0x0060)
#define GB_JOYPAD_FLAG_BIT  (4)

gb_joypad::gb_joypad(gb_memory_manager& memory_manager)
    : gb_memory_mapped_device(memory_manager, GB_JOYPAD_P1_ADDR, 1),
      gb_interrupt_source(GB_JOYPAD_JUMP_ADDR, GB_JOYPAD_FLAG_BIT)
{
    // Reset the joypad register (should be 1)
    gb_memory_mapped_device::write_byte(GB_JOYPAD_P1_ADDR, 0xFF);
}

void gb_joypad::write_byte(uint16_t addr, uint8_t val) {
    if (addr == GB_JOYPAD_P1_ADDR) {
        // Don't overwrite bits[3:0] which are the Joypad input selects
        // bits[7:6] are reserved and should be read-as-one
        uint8_t p1 = read_byte(GB_JOYPAD_P1_ADDR);
        val = (p1 & ~0x30) | (val & 0x30) | 0xc0;
    }
    gb_memory_mapped_device::write_byte(addr, val);
}

bool gb_joypad::update(int cycles) {
    return false;
}
