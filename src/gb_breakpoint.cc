#include <stdexcept>

#include "gb_breakpoint.h"

gb_breakpoint::gb_breakpoint()
    : m_breakpoints()
{
}

void gb_breakpoint::add_breakpoint(unsigned int bp, gb_breakpoint_callback_func_t& callback) {
    m_breakpoints[bp] = callback;
}

void gb_breakpoint::remove_breakpoint(unsigned int bp) {
    m_breakpoints.erase(bp);
}

void gb_breakpoint::clear_breakpoints() {
    m_breakpoints.clear();
}

bool gb_breakpoint::match_breakpoint(unsigned int val) {
    try {
        gb_breakpoint_callback_func_t& callback = m_breakpoints.at(val);
        callback(val);
    } catch (const std::exception& e) {
        return false;
    }
    return true;
}
