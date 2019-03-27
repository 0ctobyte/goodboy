#include <sstream>
#include <iomanip>

#include "gb_breakpoint.h"

gb_breakpoint_exception::gb_breakpoint_exception(unsigned int bp)
  : std::exception()
{
    std::ostringstream sstr;
    sstr << "Breakpoint hit: " << "0x" << std::hex << std::setfill('0') << std::setw(4) << bp;
    m_msg = sstr.str();
}

char const * gb_breakpoint_exception::what() const noexcept {
    return m_msg.c_str();
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

void gb_breakpoint::match(unsigned int val) {
    if (m_breakpoints.count(val) > 0) throw gb_breakpoint_exception(val);
}
