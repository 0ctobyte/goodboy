/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#include "gb_logger.h"

gb_logger& gb_logger::instance() {
    static gb_logger gbl;
    return gbl;
}

gb_logger::gb_logger()
    : std::ostream(std::cout.rdbuf()), m_log_level(GB_LOG_FATAL)
{
}

gb_logger::~gb_logger() {
}

void gb_logger::set_stream(std::ostream& o) {
    rdbuf(o.rdbuf());
}

void gb_logger::set_level(gb_logger_level_t log_level) {
    // GB_LOG_TRACE is a special separate mode, the highest log level that can be set is GB_LOG_DEBUG
    m_log_level = log_level == GB_LOG_TRACE ? GB_LOG_DEBUG : log_level;
}

void gb_logger::enable_tracing(bool enabled) {
    m_tracing = enabled;
}

gb_logger_level_t gb_logger::get_level() const {
    return m_log_level;
}

bool gb_logger::is_tracing() const {
    return m_tracing;
}
