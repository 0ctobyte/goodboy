#ifndef GB_BREAKPOINT_H_
#define GB_BREAKPOINT_H_

#include <unordered_map>
#include <functional>

using gb_breakpoint_callback_func_t = std::function<void(unsigned int)>;

// This class provides routines to add/remove and check if a breakpoint was hit
class gb_breakpoint {
friend class gb_debugger;
public:
    gb_breakpoint();

    // Add a breakpoint with a callback that's invoked if a match is found on this breakpoint
    void add_breakpoint(unsigned int bp, gb_breakpoint_callback_func_t& callback);

    // Remove the specified breakpoint
    void remove_breakpoint(unsigned int bp);

    // Remove all breakpoints
    void clear_breakpoints();

    // Given a value, check if it matches any of the registered breakpoints.
    // If so this will invoke the callback for the breakpoint
    bool match_breakpoint(unsigned int val);

private:
    using gb_breakpoint_map_t = std::unordered_map<unsigned int, gb_breakpoint_callback_func_t>;

    gb_breakpoint_map_t m_breakpoints;
};

#endif // GB_BREAKPOINT_H_
