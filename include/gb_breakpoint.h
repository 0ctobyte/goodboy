#ifndef GB_BREAKPOINT_H_
#define GB_BREAKPOINT_H_

#include <unordered_set>
#include <functional>
#include <exception>
#include <string>

class gb_breakpoint_exception : public std::exception {
public:
    gb_breakpoint_exception(std::string msg, unsigned int val);
    gb_breakpoint_exception(unsigned int val);
    gb_breakpoint_exception(unsigned int val, int cycles);

    virtual char const * what() const noexcept;
    int get_last_cycle_count() const noexcept;
    unsigned int get_val() const noexcept;

private:
    std::string  m_msg;
    unsigned int m_val;
    int          m_last_cycle_count;
};

using gb_watchpoint_exception = gb_breakpoint_exception;

// This class provides routines to add/remove and check if a breakpoint was hit
class gb_breakpoint {
friend class gb_debugger;
public:
    gb_breakpoint();

    // Add a breakpoint
    void add(unsigned int bp);

    // Remove the specified breakpoint
    void remove(unsigned int bp);

    // Remove all breakpoints
    void clear();

    // Given a value and N cycles, check if the value matches any of the registered breakpoints.
    // If so this will throw an exception of type gb_breakpoint_exception with the cycle count.
    // This is useful for the debugger to keep an accurate cycle count since the exception will be thrown
    // before the CPU returns the cycle count of the previous instruction to the breakpoint
    void match(unsigned int val, int cycles);

    // Given a value, check if it matches any of the registered breakpoints.
    // If so this will throw an exception of type gb_breakpoint_exception
    void match(unsigned int val);

private:
    using gb_breakpoint_set_t = std::unordered_set<unsigned int>;

    gb_breakpoint_set_t m_breakpoints;
};

using gb_watchpoint = gb_breakpoint;

#endif // GB_BREAKPOINT_H_
