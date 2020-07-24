/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#ifndef GB_RTC_H_
#define GB_RTC_H_

#include "gb_memory_mapped_device.h"

class gb_rtc : public gb_memory_mapped_device {
public:
    gb_rtc(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size);

    virtual uint8_t read_byte(uint16_t addr) override;
    virtual void write_byte(uint16_t addr, uint8_t val) override;
    void set_current_register(uint8_t reg);
    void update();

private:
    enum gb_rtc_register_t {
        GB_RTC_SECONDS = 0,
        GB_RTC_MINUTES = 1,
        GB_RTC_HOURS   = 2,
        GB_RTC_DAY_LO  = 3,
        GB_RTC_DAY_HI  = 4
    };

    gb_rtc_register_t m_current_register;
};

using gb_rtc_ptr = std::shared_ptr<gb_rtc>;

#endif // GB_RTC_H_
