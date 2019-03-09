#ifndef GB_LOGGER_H_
#define GB_LOGGER_H_

#include <iostream>

#define GB_LOGGER(x) if (x > gb_logger::instance().get_level()) {} else gb_logger::instance()

enum gb_logger_level_t {
    GB_LOG_FATAL,
    GB_LOG_ERROR,
    GB_LOG_WARN,
    GB_LOG_INFO,
    GB_LOG_DEBUG
};

class gb_logger : public std::ostream {
public:
    static gb_logger& instance();
    void set_stream(std::ostream& o);
    void set_level(gb_logger_level_t log_level);
    gb_logger_level_t get_level();

protected:
    gb_logger();
    virtual ~gb_logger();

private:
    gb_logger_level_t m_log_level;
};

#endif // GB_LOGGER_H_
