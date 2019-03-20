#ifndef GB_DMA_H_
#define GB_DMA_H_

#include "gb_memory_mapped_device.h"
#include "gb_memory_map.h"

class gb_dma : public gb_memory_mapped_device {
public:
    gb_dma(gb_memory_manager& memory_manager, gb_memory_map& memory_map, gb_memory_mapped_device_ptr& oam);

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual bool update(int cycles);

private:
    gb_memory_map&              m_memory_map;
    gb_memory_mapped_device_ptr m_oam;
    int                         m_bytes_transferred;
    int                         m_delay_cycles;
};

using gb_dma_ptr = std::shared_ptr<gb_dma>;

#endif // GB_DMA_H_
