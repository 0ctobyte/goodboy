#ifndef GB_INTERRUPT_SOURCE_H_
#define GB_INTERRUPT_SOURCE_H_

#include <cstdint>
#include <memory>

class gb_interrupt_source {
public:
    gb_interrupt_source(uint16_t jump_address, uint8_t flag_bit);
    virtual ~gb_interrupt_source();

    virtual uint16_t get_jump_address();
    virtual uint8_t get_flag_mask();
    virtual bool update(int cycles);

protected:
    uint16_t m_jump_address;
    uint8_t  m_flag_bit;
};

typedef std::shared_ptr<gb_interrupt_source> gb_interrupt_source_ptr;

#endif // GB_INTERRUPT_SOURCE_H_
