/*
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef GB_LOGGER_H_
#define GB_LOGGER_H_

#include <iostream>

#define GB_LOGGER(x) if ((x > gb_logger::instance().get_level()) && !(x == GB_LOG_TRACE && gb_logger::instance().is_tracing())) {} else gb_logger::instance()

// Log levels
// FATAL: These are events that cause the program to terminate
// ERROR: Bad state but program may continue but may behave incorrectly
// WARN: Warnings indicating potentially erroneous situations
// INFO: Just general information
// DEBUG: Log all debug messages
// TRACE: special mode to trace CPU instructions. This mode is separate from the other modes
enum gb_logger_level_t {
    GB_LOG_FATAL,
    GB_LOG_ERROR,
    GB_LOG_WARN,
    GB_LOG_INFO,
    GB_LOG_DEBUG,
    GB_LOG_TRACE
};

class gb_logger : public std::ostream {
public:
    static gb_logger& instance();
    void set_stream(std::ostream& o);
    void set_level(gb_logger_level_t log_level);
    void enable_tracing(bool enabled);
    gb_logger_level_t get_level() const;
    bool is_tracing() const;

protected:
    gb_logger();
    virtual ~gb_logger();

private:
    gb_logger_level_t m_log_level;
    bool              m_tracing;
};

#endif // GB_LOGGER_H_
