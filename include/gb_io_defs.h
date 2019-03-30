#ifndef GB_IO_DEFS_H_
#define GB_IO_DEFS_H_

#define CLOCK_SPEED              (4194304)

#define GB_ROM_ADDR              (0x0000)
#define GB_ROM_BANKN_ADDR        (0x4000)
#define GB_ROM_BANK_SIZE         (0x4000)

#define GB_RAM_ADDR              (0xA000)
#define GB_RAM_BANK_SIZE         (0x2000)

#define GB_VIDEO_RAM_ADDR        (0x8000)
#define GB_VIDEO_RAM_SIZE        (0x2000)
#define GB_PPU_OAM_ADDR          (0xFE00)
#define GB_PPU_OAM_SIZE          (0x00A0)

#define GB_JOYPAD_P1_ADDR        (0xFF00)

#define GB_SERIAL_IO_SB_ADDR     (0xFF01)
#define GB_SERIAL_IO_SC_ADDR     (0xFF02)

#define GB_TIMER_DIV_ADDR        (0xFF04)
#define GB_TIMER_TIMA_ADDR       (0xFF05)
#define GB_TIMER_TMA_ADDR        (0xFF06)
#define GB_TIMER_TAC_ADDR        (0xFF07)

#define GB_LCDC_ADDR             (0xFF40)
#define GB_LCD_STAT_ADDR         (0xFF41)
#define GB_LCD_LY_ADDR           (0xFF44)
#define GB_LCD_LYC_ADDR          (0xFF45)

#define GB_DMA_ADDR              (0xFF46)

#define GB_PPU_BG_SCROLL_Y_ADDR  (0xFF42)
#define GB_PPU_BG_SCROLL_X_ADDR  (0xFF43)
#define GB_PPU_BGP_ADDR          (0xFF47)
#define GB_PPU_OBP0_ADDR         (0xFF48)
#define GB_PPU_OBP1_ADDR         (0xFF49)
#define GB_PPU_WIN_SCROLL_Y_ADDR (0xFF4A)
#define GB_PPU_WIN_SCROLL_X_ADDR (0xFF4B)

#define GB_IFLAGS_ADDR           (0xFF0F)
#define GB_IENABLE_ADDR          (0xFFFF)

#endif // GB_IO_DEFS_H_
