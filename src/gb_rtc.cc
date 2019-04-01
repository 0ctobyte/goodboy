#include <chrono>

#include "gb_rtc.h"

gb_rtc::gb_rtc(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size)
    : gb_memory_mapped_device(memory_manager), m_current_register(GB_RTC_SECONDS)
{
    m_start_addr = start_addr;
    m_size = size;

    // Allocate 5 bytes for the 5 RTC registers
    m_mm_start_addr = m_memory_manager.allocate(5);

    // Update RTC registers
    update();
}

uint8_t gb_rtc::read_byte(uint16_t addr) {
    translate(addr);
    return m_memory_manager.read_byte(m_mm_start_addr + m_current_register);
}

void gb_rtc::write_byte(uint16_t addr, uint8_t val) {
    translate(addr);
    m_memory_manager.write_byte(m_mm_start_addr + m_current_register, val);
}

void gb_rtc::set_current_register(uint8_t reg) {
    switch (reg) {
        case 0x8: m_current_register = GB_RTC_SECONDS; break;
        case 0x9: m_current_register = GB_RTC_MINUTES; break;
        case 0xa: m_current_register = GB_RTC_HOURS; break;
        case 0xb: m_current_register = GB_RTC_DAY_LO; break;
        case 0xc: m_current_register = GB_RTC_DAY_HI; break;
        default: break;
    }
}

void gb_rtc::update() {
    // Update the RTC registers with the time since epoch
    unsigned long time = static_cast<unsigned long>(std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::seconds(1));
    uint8_t seconds = time % 60;
    uint8_t minutes = (time / 60) % 60;
    uint8_t hours =(time / 3600) % 24;
    uint16_t days = (time / 86400) % 512;

    m_memory_manager.write_byte(m_mm_start_addr + GB_RTC_SECONDS, seconds);
    m_memory_manager.write_byte(m_mm_start_addr + GB_RTC_MINUTES, minutes);
    m_memory_manager.write_byte(m_mm_start_addr + GB_RTC_HOURS, hours);
    m_memory_manager.write_byte(m_mm_start_addr + GB_RTC_DAY_LO, static_cast<uint8_t>(days & 0xff));
    m_memory_manager.write_byte(m_mm_start_addr + GB_RTC_DAY_HI, static_cast<uint8_t>((days > 8) & 0x1));
}
