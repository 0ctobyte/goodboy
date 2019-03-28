#include <sstream>
#include <iomanip>

#include "gb_breakpoint.h"

gb_breakpoint_exception::gb_breakpoint_exception(std::string msg, unsigned int val)
    : std::exception(), m_val(val), m_last_cycle_count(0)
{
    std::ostringstream sstr;
    sstr << msg << " 0x" << std::hex << std::setfill('0') << std::setw(4) << m_val;
    m_msg = sstr.str();
}

gb_breakpoint_exception::gb_breakpoint_exception(unsigned int val)
  : gb_breakpoint_exception("Breakpoint hit:", val)
{
}

gb_breakpoint_exception::gb_breakpoint_exception(unsigned int val, int cycles)
    : gb_breakpoint_exception(val)
{
    m_last_cycle_count = cycles;
}

char const * gb_breakpoint_exception::what() const noexcept {
    return m_msg.c_str();
}

int gb_breakpoint_exception::get_last_cycle_count() const noexcept {
    return m_last_cycle_count;
}

unsigned int gb_breakpoint_exception::get_val() const noexcept {
    return m_val;
}

gb_breakpoint::gb_breakpoint()
    : m_breakpoints()
{
}

void gb_breakpoint::add(unsigned int bp) {
    m_breakpoints.insert(bp);
}

void gb_breakpoint::remove(unsigned int bp) {
    m_breakpoints.erase(bp);
}

void gb_breakpoint::clear() {
    m_breakpoints.clear();
}
void gb_breakpoint::match(unsigned int val, int cycles) {
    if (m_breakpoints.count(val) > 0) throw gb_breakpoint_exception(val, cycles);
}

void gb_breakpoint::match(unsigned int val) {
    match(val, 0);
}
