#include "gb_ppu.h"
#include "gb_logger.h"

#define GB_VIDEO_RAM_ADDR (0x8000)
#define GB_VIDEO_RAM_SIZE (0x2000)

#define GB_PPU_BG_SCROLL_Y_ADDR  (0xFF42)
#define GB_PPU_BG_SCROLL_X_ADDR  (0xFF43)
#define GB_PPU_BGP_ADDR          (0xFF47)
//#define GB_PPU_OBP0_ADDR         (0xFF48)
//#define GB_PPU_OBP1_ADDR         (0xFF49)
#define GB_PPU_WIN_SCROLL_Y_ADDR (0xFF4A)
//#define GB_PPU_WIN_SCROLL_X_ADDR (0xFF4B)

#define GB_PPU_VBLANK_JUMP_ADDR  (0x40)
#define GB_PPU_VBLANK_FLAG_BIT   (0x0)

#define GB_LCDC_ADDR             (0xFF40)
//#define GB_LCD_STAT_ADDR         (0xFF41)
#define GB_LCD_LY_ADDR           (0xFF44)
//#define GB_LCD_LYC_ADDR          (0xFF45)

gb_ppu::gb_ppu(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_framebuffer& framebuffer)
    : gb_memory_mapped_device(memory_manager, GB_VIDEO_RAM_ADDR, GB_VIDEO_RAM_SIZE),
      gb_interrupt_source(GB_PPU_VBLANK_JUMP_ADDR, GB_PPU_VBLANK_FLAG_BIT),
      m_ppu_bg_scroll(std::make_shared<gb_memory_mapped_device>(memory_manager, GB_PPU_BG_SCROLL_Y_ADDR, 2)),
      m_ppu_palette(std::make_shared<gb_memory_mapped_device>(memory_manager, GB_PPU_BGP_ADDR, 3)),
      m_ppu_win_scroll(std::make_shared<gb_memory_mapped_device>(memory_manager, GB_PPU_WIN_SCROLL_Y_ADDR, 2)),
      m_memory_map(memory_map), m_framebuffer(framebuffer), m_next_line(0)
{
    // Add the PPU registers to the memory map
    gb_address_range_t addr_range = m_ppu_bg_scroll->get_address_range();
    memory_map.add_readable_device(m_ppu_bg_scroll, std::get<0>(addr_range), std::get<1>(addr_range));
    memory_map.add_writeable_device(m_ppu_bg_scroll, std::get<0>(addr_range), std::get<1>(addr_range));

    addr_range = m_ppu_palette->get_address_range();
    memory_map.add_readable_device(m_ppu_palette, std::get<0>(addr_range), std::get<1>(addr_range));
    memory_map.add_writeable_device(m_ppu_palette, std::get<0>(addr_range), std::get<1>(addr_range));

    addr_range = m_ppu_win_scroll->get_address_range();
    memory_map.add_readable_device(m_ppu_win_scroll, std::get<0>(addr_range), std::get<1>(addr_range));
    memory_map.add_writeable_device(m_ppu_win_scroll, std::get<0>(addr_range), std::get<1>(addr_range));
}

gb_ppu::~gb_ppu() {
}

void gb_ppu::_draw_background(uint8_t ly) {
    uint8_t lcdc = m_memory_map.read_byte(GB_LCDC_ADDR);
    uint8_t scx = m_ppu_bg_scroll->read_byte(GB_PPU_BG_SCROLL_X_ADDR);
    uint8_t scy = m_ppu_bg_scroll->read_byte(GB_PPU_BG_SCROLL_Y_ADDR);

    uint16_t background_tile_map_addr = (lcdc & 0x8) ? 0x9c00 : 0x9800;
    uint16_t background_tile_data_sel_addr = (lcdc & 0x10) ? 0x8000 : 0x9000;

    uint8_t pixel_y = (scy + ly) & 0xff;
    uint8_t tile_y = pixel_y >> 3;
    uint8_t tile_pixel_y_offset = (pixel_y & 0x7) * 2;

    // The background tile map makes up 256x256 pixels (only 160x144 are shown on the screen depending on SCX and SCY)
    // The background tile map is laid out as 32x32 tiles. Each byte refers to a tile number.
    // First convert the pixel x & y coordinates to tile coordinates tx, ty
    // The background tile map start either at 0x9c00 or 0x9800 depending on LCDC[3]
    auto get_tile_num = [this](uint16_t bgtm, uint8_t tx, uint8_t ty) -> uint8_t {
        return this->read_byte(bgtm + ((ty * 32) + tx));
    };

    // The tile number is used to index into the background tile data select array
    // There's two of them, but one of the used at a time depending on LCDC[4]
    // Starting at 0x8000, the tile number is used as an unsigned index between 0-255
    // Starting at 0x9000, the tile number is used as a signed index between -127-127
    // Each tile is 8x8 pixels, composed of 16 bytes, 2 bytes per line of 8 pixels.
    auto get_tile_data_addr = [lcdc](uint16_t bgtds, uint8_t tile_num) -> uint16_t {
        if (lcdc & 0x10) return (bgtds + (tile_num * 16));
        else return static_cast<uint16_t>(static_cast<int>(bgtds) + (static_cast<int8_t>(tile_num) * 16));
    };

    // A tiles pixel colour is composed of two bits, one from the upper byte and the other from the lower byte of the line
    // Suppos if tile# == 0, the 16 bytes of the tile 0 data may look something like this:
    // 0x8000: 01101001
    // 0x8001: 10010110
    // 0x8002: 01101001
    // 0x8003: 10010110
    // 0x8004: 01101001
    // 0x8005: 10010110
    // 0x8006: 01101001
    // 0x8007: 10010110
    // 0x8008: 01101001
    // 0x8009: 10010110
    // 0x800a: 01101001
    // 0x800b: 10010110
    // 0x800c: 01101001
    // 0x800d: 10010110
    // 0x800e: 01101001
    // 0x800f: 10010110
    // Then pixel data for line 0 of the tile:
    // X pixel: 7 6 5 4 3 2 1 0
    // ------------------------
    // 0x8000:  0 1 1 0 1 0 0 1
    // 0x8001:  1 0 0 1 0 1 1 0
    // ------------------------
    // Colour information:
    // pixel[0] = 10
    // pixel[1] = 01
    // pixel[2] = 01
    // pixel[3] = 10
    // pixel[4] = 01
    // pixel[5] = 10
    // pixel[6] = 10
    // pixel[7] = 01
    auto get_tile_pixel_colour = [this, tile_pixel_y_offset](uint16_t tile_data_addr, uint8_t tile_pixel_x) -> uint8_t {
        uint8_t tile_line_lo = this->read_byte(tile_data_addr + tile_pixel_y_offset);
        uint8_t tile_line_hi = this->read_byte(tile_data_addr + tile_pixel_y_offset+ 1);
        uint8_t col_idx = static_cast<uint8_t>(((tile_line_lo >> (7-tile_pixel_x)) & 0x1) | (((tile_line_hi >> (7-tile_pixel_x)) & 0x1) << 1));
        return ((this->m_ppu_palette->read_byte(GB_PPU_BGP_ADDR) >> (col_idx * 2)) & 0x3);
    };

    for (uint8_t lx = 0; lx < 160; lx++) {
        uint8_t pixel_x = (scx + lx) & 0xff;
        uint8_t tile_x = pixel_x >> 3;
        uint8_t tile_pixel_x = pixel_x & 0x7;
        uint16_t tile_data_addr = get_tile_data_addr(background_tile_data_sel_addr, get_tile_num(background_tile_map_addr, tile_x, tile_y));

        // If the background is turned off (LCDC[0]) just draw white
        uint8_t colour = (lcdc & 0x1) ? get_tile_pixel_colour(tile_data_addr, tile_pixel_x) : 0;

        m_framebuffer.set_pixel(lx, ly, static_cast<gb_colour_t>(colour));
    }
}

bool gb_ppu::update(int cycles) {
    bool interrupt = false;
    uint8_t ly = m_memory_map.read_byte(GB_LCD_LY_ADDR);

    // Assert the V-blank interrupt
    if (ly == 144 && m_next_line == ly) interrupt = true;

    // Draw the next scan line of the background; 160 pixels per scanline
    if (m_next_line == ly && ly < 144) _draw_background(ly);

    // Update next scan line
    m_next_line = ly + 1;

    return interrupt;
}
