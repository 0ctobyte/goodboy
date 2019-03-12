#ifndef GB_SERIAL_IO_H_
#define GB_SERIAL_IO_H_

#include <string>

#include "gb_memory_mapped_device.h"
#include "gb_interrupt_source.h"

class gb_serial_io : public gb_memory_mapped_device, public gb_interrupt_source {
public:
    gb_serial_io(gb_memory_manager& memory_manager);
    virtual ~gb_serial_io() override;

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual bool update(int cycles) override;

private:
    std::string m_str;
    int         m_irq_counter;
};

using gb_serial_io_ptr = std::shared_ptr<gb_serial_io>;

#endif // GB_SERIAL_IO_H_
