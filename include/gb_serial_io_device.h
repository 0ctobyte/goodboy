#ifndef GB_SERIAL_IO_DEVICE_H_
#define GB_SERIAL_IO_DEVICE_H_

#include <string>
#include <tuple>

#include "gb_memory_mapped_device.h"
#include "gb_interrupt_source.h"

class gb_serial_io_device : public gb_memory_mapped_device, public gb_interrupt_source {
public:
    gb_serial_io_device();
    virtual ~gb_serial_io_device();

    virtual void write_byte(uint16_t addr, uint8_t val);
    virtual bool update(int cycles);

private:
    std::string m_str;
    int         m_irq_counter;
};

#endif // GB_SERIAL_IO_DEVICE_H_
