/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#include "gb_dma.h"
#include "gb_io_defs.h"
#include "gb_logger.h"

gb_dma::gb_dma(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_memory_mapped_device_ptr& oam)
    : gb_memory_mapped_device(memory_manager, GB_DMA_ADDR, 1),
      m_memory_map(memory_map), m_oam(oam), m_bytes_transferred(160), m_delay_cycles(0)
{
}

void gb_dma::write_byte(uint16_t addr, uint8_t val) {
    if (addr == GB_DMA_ADDR) {
        // DMA transfers trigger start/restart on every write
        m_bytes_transferred = 0;
        m_delay_cycles = 4;
    }
    gb_memory_mapped_device::write_byte(addr, val);
}

bool gb_dma::update(int cycles) {
    m_memory_map.read_byte(GB_DMA_ADDR);
    // If m_bytes is at 160 that means the previous transfer has completed
    if (m_bytes_transferred == 160) return false;

    // Otherwise a transfer has started or is in progress
    // We must wait four delay cycles before before continuing
    int temp = m_delay_cycles;
    m_delay_cycles = std::max(0, m_delay_cycles - cycles);
    int n_cycles = std::max(0, cycles - temp);

    if (n_cycles <= 0) return false;

    // For every four cycles transfer 1 byte from memory to the OAM
    // The DMA register contains the MSB of the source address
    uint16_t src_addr = static_cast<uint16_t>(read_byte(GB_DMA_ADDR) << 8);
    for (int i = n_cycles; i > 0 && m_bytes_transferred < 160; i -= 4, m_bytes_transferred++) {
        uint8_t src_byte = m_memory_map.read_byte(static_cast<uint16_t>(src_addr + m_bytes_transferred));
        m_oam->write_byte(static_cast<uint16_t>(GB_PPU_OAM_ADDR + m_bytes_transferred), src_byte);
    }

    return false;
}
