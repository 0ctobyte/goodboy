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
    m_log_level = log_level;
}

gb_logger_level_t gb_logger::get_level() {
    return m_log_level;
}
