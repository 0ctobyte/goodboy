#ifndef GB_JOYPAD_H_
#define GB_JOYPAD_H_

#include "gb_memory_mapped_device.h"
#include "gb_interrupt_source.h"

class gb_joypad : public gb_memory_mapped_device, public gb_interrupt_source {
public:
    gb_joypad(gb_memory_manager& memory_manager);

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual bool update(int cycles) override;
};

using gb_joypad_ptr = std::shared_ptr<gb_joypad>;

#endif // GB_JOYPAD_H_
