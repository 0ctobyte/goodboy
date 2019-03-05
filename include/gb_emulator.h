#ifndef GB_EMULATOR_H_
#define GB_EMULATOR_H_

#include <cstdint>
#include <string>

#include "gb_cpu.h"

class gb_emulator {
public:
    gb_emulator(std::string rom_filename);

    virtual ~gb_emulator();

    virtual bool go();

protected:
    gb_cpu             m_cpu;
    uint64_t           m_cycles;
    int                m_binsize;

};

#endif // GB_EMULATOR_H_
