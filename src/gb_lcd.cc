#include "gb_lcd.h"

#define GB_LCD_STAT_JUMP_ADDR (0x48)
#define GB_LCD_FLAG_BIT       (0x1)

#define GB_LCDC_ADDR          (0xFF40)
#define GB_LCD_STAT_ADDR      (0xFF41)
#define GB_LCD_LY_ADDR        (0xFF44)
#define GB_LCD_LYC_ADDR       (0xFF45)

gb_lcd::gb_lcd_stat_register::gb_lcd_stat_register(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size)
    : gb_memory_mapped_device(memory_manager, start_addr, size)
{
}

gb_lcd::gb_lcd_stat_register::~gb_lcd_stat_register() {
}

void gb_lcd::gb_lcd_stat_register::write_byte(uint16_t addr, uint8_t val) {
    // The LCD stat register bit 7 is reserved and read-as-one
    val |= 0x80;
    gb_memory_mapped_device::write_byte(addr, val);
}

gb_lcd::gb_lcd_ly_register::gb_lcd_ly_register(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size)
    : gb_memory_mapped_device(memory_manager, start_addr, size)
{
}

gb_lcd::gb_lcd_ly_register::~gb_lcd_ly_register() {
}

void gb_lcd::gb_lcd_ly_register::write_byte(uint16_t addr, uint8_t val) {
    if (addr == GB_LCD_LY_ADDR) {
        // Writing to the LY register resets the counter to 0
        val = 0;
    }
    gb_memory_mapped_device::write_byte(addr, val);
}

void gb_lcd::gb_lcd_ly_register::set_ly(uint8_t ly) {
    gb_memory_mapped_device::write_byte(GB_LCD_LY_ADDR, ly);
}

gb_lcd::gb_lcd(gb_memory_manager& memory_manager, gb_memory_map& memory_map)
    : gb_memory_mapped_device(memory_manager, GB_LCDC_ADDR, 1),
      gb_interrupt_source(GB_LCD_STAT_JUMP_ADDR, GB_LCD_FLAG_BIT),
      m_lcd_stat(std::make_shared<gb_lcd_stat_register>(memory_manager, GB_LCD_STAT_ADDR, 1)),
      m_lcd_ly(std::make_shared<gb_lcd_ly_register>(memory_manager, GB_LCD_LY_ADDR, 2)),
      m_scanline_counter(0)
{
    // Add the LCD registers to the memory map
    gb_address_range_t addr_range = m_lcd_stat->get_address_range();
    memory_map.add_readable_device(m_lcd_stat, std::get<0>(addr_range), std::get<1>(addr_range));
    memory_map.add_writeable_device(m_lcd_stat, std::get<0>(addr_range), std::get<1>(addr_range));

    addr_range = m_lcd_ly->get_address_range();
    memory_map.add_readable_device(m_lcd_ly, std::get<0>(addr_range), std::get<1>(addr_range));
    memory_map.add_writeable_device(m_lcd_ly, std::get<0>(addr_range), std::get<1>(addr_range));
}

gb_lcd::~gb_lcd() {
}

void gb_lcd::write_byte(uint16_t addr, uint8_t val) {
    // Reset LY if the LCD is being turned on
    if (addr == GB_LCDC_ADDR && (read_byte(GB_LCDC_ADDR) & 0x80) == 0 && (val & 0x80)) {
        m_lcd_ly->set_ly(0);
    }
    gb_memory_mapped_device::write_byte(addr, val);
}

bool gb_lcd::update(int cycles) {
    bool interrupt = false;
    uint8_t ly = m_lcd_ly->read_byte(GB_LCD_LY_ADDR);
    uint8_t lyc = m_lcd_ly->read_byte(GB_LCD_LYC_ADDR);
    uint8_t lcd_stat = m_lcd_stat->read_byte(GB_LCD_STAT_ADDR);
    uint8_t mode = lcd_stat & 0x3;

    // Increment the Line counter every 456 clocks
    m_scanline_counter += cycles;
    if (m_scanline_counter >= 456) {
        m_scanline_counter = m_scanline_counter - 456;
        ly = (ly + 1) % 154;
        m_lcd_ly->set_ly(ly);

        // Check the LYC comparison & set the LYC=LY bit in the LCD_STAT register
        // Don't interrupt if LCD_STAT[6] == 0
        if (ly == lyc) {
            lcd_stat = lcd_stat | 0x4;
            interrupt = (lcd_stat & 0x40) ? true : interrupt;
        }
    }

    if (ly >= 144) {
        // LCD is in V-blank mode between scanlines 144-153
        // Don't interrupt if LCD_STAT[4] == 0
        interrupt = (mode != 1 && (lcd_stat & 0x10)) ? true : interrupt;
        lcd_stat = (lcd_stat ^ mode) | 1;
    } else if (m_scanline_counter < 80) {
        // The first 80 clocks of a scanline the LCD is in mode 2 (searching OAM, OAM is not accessible)
        // Don't interrupt if LCD_STAT[5] == 0
        interrupt = (mode != 2 && (lcd_stat & 0x20)) ? true : interrupt;
        lcd_stat = (lcd_stat ^ mode) | 2;
    } else if (m_scanline_counter < 204) {
        // LCD is in mode 3 between 80 - 204 clocks (transferring VRAM and OAM data to LCD, VRAM & OAM is not accessible)
        // No interrupt for mode 3
        lcd_stat = (lcd_stat ^ mode) | 3;
    } else if (m_scanline_counter < 456) {
        // LCD is in mode 0 between 204 - 456 clocks (H-blank)
        // Don't interrupt if LCD_STAT[3] == 0
        interrupt = (mode != 0 && (lcd_stat & 0x8)) ? true : interrupt;
        lcd_stat = (lcd_stat ^ mode);
    }

    // Update the LCD STAT register
    m_lcd_stat->write_byte(GB_LCD_STAT_ADDR, lcd_stat);

    return interrupt;
}
