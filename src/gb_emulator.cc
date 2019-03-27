#include <stdexcept>
#include <sstream>
#include <fstream>

#include "gb_emulator.h"
#include "gb_memory_bank_controller.h"
#include "gb_ram.h"
#include "gb_serial_io.h"
#include "gb_timer.h"
#include "gb_joypad.h"
#include "gb_lcd.h"
#include "gb_ppu.h"
#include "gb_io_defs.h"

#define GB_RENDERER_WIDTH  (GB_WIDTH*5)
#define GB_RENDERER_HEIGHT (GB_HEIGHT*5)

gb_emulator::gb_emulator()
    : m_renderer(GB_RENDERER_WIDTH, GB_RENDERER_HEIGHT), m_memory_manager(), m_memory_map(), m_cpu(m_memory_map), m_interrupt_controller(m_memory_manager, m_memory_map, m_cpu), m_dma(), m_cycles(0)
{
}

gb_emulator::~gb_emulator() {
}

bool gb_emulator::_run_bootrom() {
    std::ifstream dmg_file ("DMG_ROM.bin", std::ifstream::binary);

    // If DMG_ROM.bin doesn't exist in the current directory then just skip it
    if (!dmg_file) return false;

    dmg_file.seekg(0, dmg_file.end);
    int size = std::min(static_cast<int>(dmg_file.tellg()), 0x100);
    dmg_file.seekg(0, dmg_file.beg);

    // Get the previous ROM entry
    gb_memory_mapped_device_ptr game_rom = m_memory_map.get_readable_device(0x0);

    // Save the first 256 where the bootrom will go
    std::vector<uint8_t> temp (0x100, 0);
    memcpy(temp.data(), game_rom->get_mem(), 0x100);

    // Load the DMG bootrom
    dmg_file.read(reinterpret_cast<char*>(game_rom->get_mem()), size);

    // Run the bootrom
    while (m_cpu.get_pc() < 0x100 && m_renderer.is_open()) {
        step(70224);
        m_renderer.update(((m_memory_map.read_byte(GB_LCDC_ADDR) & 0x80) != 0));
    }

    // Restore the game ROM
    memcpy(game_rom->get_mem(), temp.data(), 0x100);

    return true;
}

void gb_emulator::load_rom(const std::string& rom_filename) {
    // Initialize the memory bank controller which controls the first 32KB of ROM and the 8KB of external RAM
    // The MBC listens to writes to the ROM space (0x0 - 0x8000) and configures the ROM/RAM and other devices
    // mapped to the ROM and external RAM space inside the cartridge
    gb_memory_bank_controller_ptr mbc = std::make_shared<gb_memory_bank_controller>(m_memory_manager, m_memory_map);
    mbc->load_rom(rom_filename);
    gb_address_range_t addr_range = mbc->get_address_range();
    m_memory_map.add_writeable_device(mbc, std::get<0>(addr_range), std::get<1>(addr_range));

    // Add 8KB of work RAM (on the gameboy)
    gb_ram_ptr work_ram = std::make_shared<gb_ram>(m_memory_manager, 0xC000, 0x2000, 0x2000);
    addr_range = work_ram->get_address_range();
    m_memory_map.add_readable_device(work_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(work_ram, std::get<0>(addr_range), std::get<1>(addr_range));

    // Add Serial IO device for printing to terminal
    gb_serial_io_ptr sio = std::make_shared<gb_serial_io>(m_memory_manager);
    addr_range = sio->get_address_range();
    m_memory_map.add_readable_device(sio, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(sio, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(sio);

    // Add Timer
    gb_timer_ptr timer = std::make_shared<gb_timer>(m_memory_manager);
    addr_range = timer->get_address_range();
    m_memory_map.add_readable_device(timer, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(timer, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(timer);

    // Add Joypad
    gb_joypad_ptr joypad = std::make_shared<gb_joypad>(m_memory_manager, m_renderer.get_input());
    addr_range = joypad->get_address_range();
    m_memory_map.add_readable_device(joypad, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(joypad, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(joypad);

    // Add 128 bytes of high RAM (used for stack and temp variables)
    gb_ram_ptr high_ram = std::make_shared<gb_ram>(m_memory_manager, 0xFF80, 0x7F, 0x7F);
    addr_range = high_ram->get_address_range();
    m_memory_map.add_readable_device(high_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(high_ram, std::get<0>(addr_range), std::get<1>(addr_range));

    // Add the LCD controller and it's registers
    gb_lcd_ptr lcd = std::make_shared<gb_lcd>(m_memory_manager, m_memory_map);
    addr_range = lcd->get_address_range();
    m_memory_map.add_readable_device(lcd, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(lcd, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(lcd);

    // Add the Pixel Processing Unit and it's registers
    gb_ppu_ptr ppu = std::make_shared<gb_ppu>(m_memory_manager, m_memory_map, m_renderer.get_framebuffer());
    addr_range = ppu->get_address_range();
    m_memory_map.add_readable_device(ppu, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(ppu, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(ppu);

    // Add the DMA
    gb_memory_mapped_device_ptr oam = m_memory_map.get_readable_device(GB_PPU_OAM_ADDR);
    m_dma = std::make_shared<gb_dma>(m_memory_manager, m_memory_map, oam);
    addr_range = m_dma->get_address_range();
    m_memory_map.add_readable_device(m_dma, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(m_dma, std::get<0>(addr_range), std::get<1>(addr_range));
}

int gb_emulator::step(const int num_cycles) {
    int step_cycles = 0;
    while (step_cycles < num_cycles) {
        int cycles = m_cpu.step();
        m_cycles += static_cast<uint64_t>(cycles);
        m_interrupt_controller.update(cycles);
        m_dma->update(cycles);
        step_cycles += cycles;
    }

    return step_cycles;
}

void gb_emulator::go() {
    // Run DMG bootrom if available, otherwise skip to PC=0x0100
    if (!_run_bootrom()) m_cpu.set_pc(0x0100);

    while (m_renderer.is_open()) {
        step(70224);
        m_renderer.update(((m_memory_map.read_byte(GB_LCDC_ADDR) & 0x80) != 0));
    }
}
