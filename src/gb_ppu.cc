#include "gb_ppu.h"
#include "gb_io_defs.h"
#include "gb_logger.h"

#define GB_PPU_VBLANK_JUMP_ADDR       (0x40)
#define GB_PPU_VBLANK_FLAG_BIT        (0x0)

#define GB_PPU_BG_TILE_MAP0_ADDR      (0x9800)
#define GB_PPU_BG_TILE_MAP1_ADDR      (0x9c00)
#define GB_PPU_TILE_DATA0_BASE_ADDR   (0x9000)
#define GB_PPU_TILE_DATA1_BASE_ADDR   (0x8000)

struct gb_ppu_sprite_t {
    uint8_t  entry_num; // Entry # in the OAM, used for priority calculation
    uint8_t  y;
    uint8_t  x;
    uint8_t  tile_num;
    bool     bg_priority;
    bool     flip_y;
    bool     flip_x;
    bool     use_obp1;
};

gb_ppu::gb_ppu(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_framebuffer& framebuffer)
    : gb_memory_mapped_device(memory_manager, GB_VIDEO_RAM_ADDR, GB_VIDEO_RAM_SIZE),
      gb_interrupt_source(GB_PPU_VBLANK_JUMP_ADDR, GB_PPU_VBLANK_FLAG_BIT),
      m_ppu_bg_scroll(std::make_shared<gb_memory_mapped_device>(memory_manager, GB_PPU_BG_SCROLL_Y_ADDR, 2)),
      m_ppu_palette(std::make_shared<gb_memory_mapped_device>(memory_manager, GB_PPU_BGP_ADDR, 3)),
      m_ppu_win_scroll(std::make_shared<gb_memory_mapped_device>(memory_manager, GB_PPU_WIN_SCROLL_Y_ADDR, 2)),
      m_ppu_oam(std::make_shared<gb_memory_mapped_device>(memory_manager, GB_PPU_OAM_ADDR, GB_PPU_OAM_SIZE)),
      m_memory_map(memory_map), m_framebuffer(framebuffer), m_linebuffer(), m_next_line(0)
{
    // Add the OAM memory to the memory map
    gb_address_range_t addr_range = m_ppu_oam->get_address_range();
    memory_map.add_readable_device(m_ppu_oam, std::get<0>(addr_range), std::get<1>(addr_range));
    memory_map.add_writeable_device(m_ppu_oam, std::get<0>(addr_range), std::get<1>(addr_range));

    // Add the PPU registers to the memory map
    addr_range = m_ppu_bg_scroll->get_address_range();
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

    uint8_t background_palette = m_ppu_palette->read_byte(GB_PPU_BGP_ADDR);
    uint16_t background_tile_map_addr = (lcdc & GB_LCDC_TILE_MAP_SEL_MASK) ? GB_PPU_BG_TILE_MAP1_ADDR : GB_PPU_BG_TILE_MAP0_ADDR;
    uint16_t background_tile_data_sel_addr = (lcdc & GB_LCDC_TILE_DATA_SEL_MASK) ? GB_PPU_TILE_DATA1_BASE_ADDR : GB_PPU_TILE_DATA0_BASE_ADDR;

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
        if (lcdc & GB_LCDC_TILE_DATA_SEL_MASK) return (bgtds + (tile_num * 16));
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
    auto get_tile_pixel_colour_idx = [this, tile_pixel_y_offset](uint16_t tile_data_addr, uint8_t tile_pixel_x) -> uint8_t {
        uint8_t tile_line_lo = this->read_byte(tile_data_addr + tile_pixel_y_offset);
        uint8_t tile_line_hi = this->read_byte(tile_data_addr + tile_pixel_y_offset+ 1);
        uint8_t x = 7 - tile_pixel_x;
        return static_cast<uint8_t>(((tile_line_lo >> x) & 0x1) | (((tile_line_hi >> x) & 0x1) << 1));
    };

    for (uint8_t lx = 0; lx < 160; lx++) {
        uint8_t pixel_x = (scx + lx) & 0xff;
        uint8_t tile_x = pixel_x >> 3;
        uint8_t tile_pixel_x = pixel_x & 0x7;
        uint16_t tile_data_addr = get_tile_data_addr(background_tile_data_sel_addr, get_tile_num(background_tile_map_addr, tile_x, tile_y));

        // Get the colour index and put it in the linebuffer along with the palette
        // This will be used by the sprite rendering logic to determine background priority
        uint8_t col_idx = get_tile_pixel_colour_idx(tile_data_addr, tile_pixel_x);
        m_linebuffer.at(lx) = {GB_PPU_BG_PIXEL, static_cast<uint8_t>((lcdc & GB_LCDC_BG_WIN_ENABLE_MASK) ? col_idx : 0), background_palette};
    }
}

void gb_ppu::_draw_window(uint8_t ly) {
    uint8_t lcdc = m_memory_map.read_byte(GB_LCDC_ADDR);

    int wx = static_cast<int>(m_ppu_win_scroll->read_byte(GB_PPU_WIN_SCROLL_X_ADDR));
    int wy = static_cast<int>(m_ppu_win_scroll->read_byte(GB_PPU_WIN_SCROLL_Y_ADDR));

    // The window isn't visible if the background/window isn't enabled, the window isn't enabled,
    // wx < 0 or wx >= 167, or wy < 0 or wy >= 144
    if (!((lcdc & GB_LCDC_WIN_ENABLE_MASK) && (lcdc & GB_LCDC_BG_WIN_ENABLE_MASK) && (wx >=0 && wx < 167) && (wy >=0 && wy < 144))) return;

    // Check the scanline counter against the window scroll Y position. Don't start drawing until LY reaches WY
    if (ly < wy) return;

    uint8_t window_palette = m_ppu_palette->read_byte(GB_PPU_BGP_ADDR);
    uint16_t window_tile_map_addr = (lcdc & GB_LCDC_WIN_TILE_MAP_SEL_MASK) ? GB_PPU_BG_TILE_MAP1_ADDR : GB_PPU_BG_TILE_MAP0_ADDR;
    uint16_t window_tile_data_sel_addr = (lcdc & GB_LCDC_TILE_DATA_SEL_MASK) ? GB_PPU_TILE_DATA1_BASE_ADDR : GB_PPU_TILE_DATA0_BASE_ADDR;

    uint8_t pixel_y = static_cast<uint8_t>((wy + ly) & 0xff);
    uint8_t tile_y = pixel_y >> 3;
    uint8_t tile_pixel_y_offset = (pixel_y & 0x7) * 2;

    // The window tile map is overlaid on top of the background. The WX and WY registers hold the starting position of the
    // window that will be shown on top of the background (stretching all the way to the right and to the bottom of the screen)
    auto get_tile_num = [this](uint16_t wtm, uint8_t tx, uint8_t ty) -> uint8_t {
        return this->read_byte(wtm + ((ty * 32) + tx));
    };

    // The tile number in the window tile map is used to index into one of the two tile data arrays in video RAM
    // This is pretty much similar to how the background tile data array works
    auto get_tile_data_addr = [lcdc](uint16_t wtds, uint8_t tile_num) -> uint16_t {
        if (lcdc & GB_LCDC_TILE_DATA_SEL_MASK) return (wtds + (tile_num * 16));
        else return static_cast<uint16_t>(static_cast<int>(wtds) + (static_cast<int8_t>(tile_num) * 16));
    };

    // This is again similar to how the background pixel colours work
    auto get_tile_pixel_colour_idx = [this, tile_pixel_y_offset](uint16_t tile_data_addr, uint8_t tile_pixel_x) -> uint8_t {
        uint8_t tile_line_lo = this->read_byte(tile_data_addr + tile_pixel_y_offset);
        uint8_t tile_line_hi = this->read_byte(tile_data_addr + tile_pixel_y_offset+ 1);
        uint8_t x = 7 - tile_pixel_x;
        return static_cast<uint8_t>(((tile_line_lo >> x) & 0x1) | (((tile_line_hi >> x) & 0x1) << 1));
    };

    // Start drawing the window from WX (minus 7)
    // The WIN_SCROLL_X register needs to be offset by 7
    wx -= 7;
    for (int lx = wx; lx < 160; lx++) {
        // If the pixel location is negative then skip it
        if (lx < 0) continue;

        uint8_t tile_x = static_cast<uint8_t>(lx >> 3);
        uint8_t tile_pixel_x = static_cast<uint8_t>((lx - wx) & 0x7);
        uint16_t tile_data_addr = get_tile_data_addr(window_tile_data_sel_addr, get_tile_num(window_tile_map_addr, tile_x, tile_y));
        uint8_t col_idx = get_tile_pixel_colour_idx(tile_data_addr, tile_pixel_x);

        // Push the pixel into the linebuffer
        m_linebuffer.at(static_cast<uint8_t>(lx)) = {GB_PPU_BG_PIXEL, col_idx, window_palette};
    }
}

void gb_ppu::_draw_sprites(uint8_t ly) {
    uint8_t lcdc = m_memory_map.read_byte(GB_LCDC_ADDR);

    // Don't draw sprites if LCDC[1] == 0
    if ((lcdc & GB_LCDC_SPRITE_ENABLE_MASK) == 0) return;

    // Sprites can either 8x8 pixels or 8x16 pixels depending on LCDC[2]
    bool double_size = (lcdc & GB_LCDC_SPRITE_SIZE_MASK) ? true : false;
    uint8_t sprite_size = double_size ? 16 : 8;
    uint16_t sprite_tile_data_addr = GB_VIDEO_RAM_ADDR;
    uint8_t obp0 = m_ppu_palette->read_byte(GB_PPU_OBP0_ADDR);
    uint8_t obp1 = m_ppu_palette->read_byte(GB_PPU_OBP1_ADDR);

    // Search the Object Attribute Memory and Accumulate a list of of Sprites
    // that are visible in general and are visible in the current scanline
    // The OAM consists of 4 bytes per Sprite with a total of 40 sprites supported:
    // byte0: Y position relative to the LCD screen
    // byte1: X position relative to the LCD screen
    // byte2: Pattern (Tile) number, LSB is ignored if in 8x16 sprite mode
    // byte3: Flags
    //        - [7] : BG priority; 0 means visible, 1 means visible only if current background pixel is white (i.e. colour 0)
    //        - [6] : Y flip
    //        - [5] : X flip
    //        - [4] : Palette # (1 for OBP1 and 0 for OBP0)
    // Sprites are not visible if (y == 0 || y >= (144+16)) || (x == 0 || x >= (160+8))
    // Because then sprites are basically around the edges of the screen (sprites x,y coordinates refer to the bottom right corner)
    // Sprites are visible in the current scanline iff y-sprite_size <= ly < y
    std::vector<gb_ppu_sprite_t> visible_sprites;
    visible_sprites.reserve(40);
    for (unsigned int i = 0; i < 40; i++) {
        uint16_t oam_entry_addr = static_cast<uint16_t>(GB_PPU_OAM_ADDR + (i * 4));

        uint8_t y = m_ppu_oam->read_byte(oam_entry_addr);
        uint8_t x = m_ppu_oam->read_byte(oam_entry_addr + 1);
        uint8_t tile_num = m_ppu_oam->read_byte(oam_entry_addr + 2);
        uint8_t flags = m_ppu_oam->read_byte(oam_entry_addr + 3);

        // Check if sprite is visible at all
        if (y == 0 || y >= 160 || x == 0 || x >= 168) continue;

        // Check if sprite is visible in the current scanline
        if (ly < (y - 16) || ly >= ((y - 16) + sprite_size)) continue;

        // Add sprite to vector
        tile_num = double_size ? tile_num & ~0x1 : tile_num;
        bool bg_priority = (flags & 0x80) ? true : false;
        bool flip_y = (flags & 0x40) ? true : false;
        bool flip_x = (flags & 0x20) ? true : false;
        bool use_obp1 = (flags & 0x10) ? true : false;
        visible_sprites.push_back({static_cast<uint8_t>(i), y, x, tile_num, bg_priority, flip_y, flip_x, use_obp1});

        // The Gameboy has a limitation where it can only render 10 sprites per scanline
        if (visible_sprites.size() == 10) break;
    }

    // Now we need to sort the visible sprites by priority (i.e. lowest priority first so they get drawn over)
    // Sprite A has priority over Sprite B if the following conditions apply:
    // - Sprites with the lowest x coordinates has precedence: A.x != B.x && A.x < B.x
    // - In case of same x, sprites take precedence according to OAM table ordering: A.x == B.x && A.entry_num < B.entry_num
    std::sort(visible_sprites.begin(), visible_sprites.end(),
        [](const gb_ppu_sprite_t& a, const gb_ppu_sprite_t& b) -> bool {
            return (a.x > b.x || (a.x == b.x && a.entry_num > b.entry_num));
    });

    // Now go through each visible sprite and draw the part of the scanline it's visible in
    for (gb_ppu_sprite_t& sprite : visible_sprites) {
        // Sprite coordinates refer to the bottom right corner. Translate this to top left corner
        int sx = sprite.x - 8;
        int sy = sprite.y - 16;

        // Get the pointer to the Sprite tile data
        // Then calculate the offset to the 2 bytes for the current scanline (depending on flip_y)
        uint8_t y = static_cast<uint8_t>(ly - sy);
        uint16_t sprite_addr = sprite_tile_data_addr + (sprite.tile_num * 16);
        uint8_t line_offset = (sprite.flip_y ? (sprite_size - 1) - y : y) * 2;

        // Read the pixel data for the line
        uint8_t sprite_line_lo = this->read_byte(sprite_addr + line_offset);
        uint8_t sprite_line_hi = this->read_byte(sprite_addr + (line_offset + 1));

        // Which palette is this sprite using?
        uint8_t obj_palette = sprite.use_obp1 ? obp1 : obp0;

        for (int lx = sx; lx < (sx + 8); lx++) {
            // Check if lx is out of bounds
            if (lx < 0 || lx >= 160) continue;

            // Get colour index from the tile line; works the same way as the background tile data
            uint8_t x = static_cast<uint8_t>(lx - sx);
            x = sprite.flip_x ? x : 7 - x;
            uint8_t colour_idx = static_cast<uint8_t>(((sprite_line_lo >> x) & 0x1) | (((sprite_line_hi >> x) & 0x1) << 1));

            // For sprites, a colour index of 0 in the tile data means transparent. Don't even draw it.
            if (colour_idx == 0) continue;

            // Get the colour from the object palette
            //gb_colour_t colour = static_cast<gb_colour_t>((obj_palette >> (colour_idx * 2)) & 0x3);

            // If BG priority is enabled then don't draw the pixel if the current background pixel colour is not colour 0
            // The background has priority for all other colours except colour 0 in which case the sprite pixel can be drawn
            if (sprite.bg_priority && m_linebuffer.at(static_cast<size_t>(lx)).pixel_type == GB_PPU_BG_PIXEL && m_linebuffer.at(static_cast<size_t>(lx)).colour_idx != 0) continue;

            // Push the pixel into the linebuffer
            m_linebuffer.at(static_cast<size_t>(lx)) = {GB_PPU_SPRITE_PIXEL, colour_idx, obj_palette};
        }
    }
}

bool gb_ppu::update(int cycles) {
    bool interrupt = false;
    uint8_t ly = m_memory_map.read_byte(GB_LCD_LY_ADDR);

    // Assert the V-blank interrupt if ly == 144
    if (ly == 144 && m_next_line == ly) interrupt = true;

    // Draw the next scan line of the background; 160 pixels per scanline
    // Draw the window & finally draw the sprites
    if (m_next_line == ly && ly < 144) {
        _draw_background(ly);
        _draw_window(ly);
        _draw_sprites(ly);

        // Draw linebuffer to framebuffer
        for (const gb_ppu_pixel_t& p : m_linebuffer) {
            uint8_t lx = static_cast<uint8_t>(&p - &m_linebuffer[0]);
            gb_colour_t colour = static_cast<gb_colour_t>((p.palette >> (p.colour_idx *2)) & 0x3);
            m_framebuffer.set_pixel(lx, ly, colour);
        }
    }

    // Update next scan line
    m_next_line = ly + 1;
    m_next_line = (m_next_line > 153) ? 0 : m_next_line;

    return interrupt;
}
