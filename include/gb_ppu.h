#ifndef GB_PPU_H_
#define GB_PPU_H_

#include "gb_memory_map.h"
#include "gb_interrupt_source.h"
#include "gb_framebuffer.h"

class gb_ppu : public gb_memory_mapped_device, public gb_interrupt_source {
public:
    gb_ppu(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_framebuffer& framebuffer);
    virtual ~gb_ppu() override;

    virtual bool update(int cycles) override;

private:
    // Registers used by the PPU
    // bg_scroll - SCY: BG Y Position SCX: BG X Position
    // palette - BGP: Background & Window Palette Data OBP0: Object Palette 0 OBP1: Object Palette 1
    // bits[7:6] = Colour3
    // bits[5:4] = Colour2
    // bits[3:2] = Colour1
    // bits[1:0] = Colour0
    // win_scroll - WY: Window Y Position (0-143) WX: Window X Position (-7; 0-166))
    gb_memory_mapped_device_ptr m_ppu_bg_scroll;
    gb_memory_mapped_device_ptr m_ppu_palette;
    gb_memory_mapped_device_ptr m_ppu_win_scroll;

    // Need a reference to the memory map to access LCD registers
    gb_memory_map&              m_memory_map;

    // Reference to framebuffer where pixels will be drawn
    gb_framebuffer&             m_framebuffer;

    // Previous scanline
    int                         m_next_line;

    void _draw_background(uint8_t ly);
};

using gb_ppu_ptr = std::shared_ptr<gb_ppu>;

#endif // GB_PPU_H_
