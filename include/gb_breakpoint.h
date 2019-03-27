#ifndef GB_BREAKPOINT_H_
#define GB_BREAKPOINT_H_

#include <unordered_set>
#include <functional>
#include <exception>
#include <string>

class gb_breakpoint_exception : public std::exception {
public:
    gb_breakpoint_exception(unsigned int bp);

    virtual char const * what() const noexcept;

private:
    std::string m_msg;
};

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

    // Given a value, check if it matches any of the registered breakpoints.
    // If so this will throw an exception of type gb_breakpoint_exception
    void match(unsigned int val);

private:
    using gb_breakpoint_set_t = std::unordered_set<unsigned int>;

    gb_breakpoint_set_t m_breakpoints;
};

#endif // GB_BREAKPOINT_H_
