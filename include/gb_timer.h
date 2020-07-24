/*
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef GB_TIMER_H_
#define GB_TIMER_H_

#include <array>

#include "gb_memory_mapped_device.h"
#include "gb_interrupt_source.h"

class gb_timer : public gb_memory_mapped_device, public gb_interrupt_source {
public:
    gb_timer(gb_memory_manager& memory_manager);
    virtual ~gb_timer() override;

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual bool update(int cycles) override;

private:
    using gb_timer_clk_select_tbl_t = std::array<int, 4>;

    const gb_timer_clk_select_tbl_t m_timer_clk_select_tbl;
    int                             m_div_counter;
    int                             m_timer_counter;
    bool                            m_timer_start;
    unsigned int                    m_timer_clk_select;
};

using gb_timer_ptr = std::shared_ptr<gb_timer>;

#endif // GB_TIMER_H_
