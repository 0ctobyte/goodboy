#include "gb_ppu.h"

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

#include "gb_logger.h"
bool gb_ppu::update(int cycles) {
    bool interrupt = false;
    uint8_t lcdc = m_memory_map.read_byte(GB_LCDC_ADDR);
    uint8_t ly = m_memory_map.read_byte(GB_LCD_LY_ADDR);
    uint8_t scx = m_ppu_bg_scroll->read_byte(GB_PPU_BG_SCROLL_X_ADDR);
    uint8_t scy = m_ppu_bg_scroll->read_byte(GB_PPU_BG_SCROLL_Y_ADDR);

    // Assert the V-blank interrupt
    if (ly == 144 && m_next_line == ly) {
        interrupt = true;
    }

    if (m_next_line == ly && ly < 144) {
        // Draw the next scan line
        // 160 pixels in each scanline
        for (uint8_t lx = 0; lx < 160; lx++) {
            GB_LOGGER(GB_LOG_INFO) << "scx: " << std::dec << static_cast<unsigned>(scx) << " scy: " << std::dec << static_cast<unsigned>(scy) << " lcdc: " << std::hex << static_cast<uint16_t>(lcdc) << std::endl;
            GB_LOGGER(GB_LOG_INFO) << "lx: " << std::dec << static_cast<unsigned>(lx) << " ly: " << std::dec << static_cast<unsigned>(ly) << std::endl;
            uint8_t rx = (scx + lx) & 0xff;
            uint8_t ry = (scy + ly) & 0xff;
            uint8_t tx = rx >> 3;
            uint8_t ty = ry >> 3;
            uint16_t bgt = (lcdc & 0x8) ? 0x9C00 : 0x9800;
            GB_LOGGER(GB_LOG_INFO) << "rx: " << std::dec << static_cast<unsigned>(rx) << " ry: " << std::dec << static_cast<unsigned>(ry) << std::endl;
            GB_LOGGER(GB_LOG_INFO) << "tx: " << std::dec << static_cast<unsigned>(tx) << " ty: " << std::dec << static_cast<unsigned>(ty) << std::endl;
            GB_LOGGER(GB_LOG_INFO) << "bgt: " << std::hex << bgt << " tile_num_addr: " << std::hex << (bgt + ((ty * 32) + tx)) << std::endl;
            uint8_t tile_num = this->read_byte(bgt + ((ty * 32) + tx));
            int bgtds = (lcdc & 0x10) ? 0x8000 : 0x9000;
            uint16_t tile_data = static_cast<uint16_t>((lcdc & 0x10) ? static_cast<unsigned>(bgtds) + (tile_num * 16) : static_cast<unsigned>(bgtds + (static_cast<int8_t>(tile_num) * 16)));
            GB_LOGGER(GB_LOG_INFO) << "tile_num: " << std::hex << static_cast<int>(tile_num) << " bgtds: " << std::hex << bgtds << " tile_data: " << std::hex << tile_data << std::endl;
            uint8_t tpx = rx & 0x7;
            uint8_t tpy = ry & 0x7;
            GB_LOGGER(GB_LOG_INFO) << "tpx: " << std::dec << static_cast<int>(tpx) << " tpy: " << std::dec << static_cast<int>(tpy) << std::endl;
            GB_LOGGER(GB_LOG_INFO) << "tdblo_addr: " << std::hex << (tile_data + (tpy * 2)) << " tdbhi_addr: " << std::hex << (tile_data + ((tpy * 2) + 1)) << std::endl;
            uint8_t tile_line_lo = this->read_byte(tile_data + (tpy * 2));
            uint8_t tile_line_hi = this->read_byte(tile_data + ((tpy * 2) + 1));
            uint8_t col_idx = static_cast<uint8_t>(((tile_line_lo >> (7-tpx)) & 0x1) | (((tile_line_hi >> (7-tpx)) & 0x1) << 1));
            uint8_t bg_colour = (m_ppu_palette->read_byte(GB_PPU_BGP_ADDR) >> (col_idx * 2)) & 0x3;
            GB_LOGGER(GB_LOG_INFO) << "tile_line_lo: " << std::hex << static_cast<int>(tile_line_lo) << " tile_line_hi: " << std::hex << static_cast<int>(tile_line_hi) << std::endl;
            GB_LOGGER(GB_LOG_INFO) << " col_idx: " << static_cast<int>(col_idx) << " bg_colour: " << static_cast<int>(bg_colour) << std::endl;
            m_framebuffer.set_pixel(lx, ly, static_cast<gb_colour_t>(bg_colour));
        }
    }

    // Update next scan line
    m_next_line = ly + 1;

    return interrupt;
}
