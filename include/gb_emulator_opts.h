/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#ifndef GB_EMULATOR_OPTS_H_
#define GB_EMULATOR_OPTS_H_

#include <string>
#include <unordered_map>
#include <functional>
#include <array>

class gb_emulator_opts {
public:
    std::string m_program_name;
    std::string m_rom_filename;
    bool        m_debugger;
    bool        m_tracing;

    gb_emulator_opts(int argc, char **argv);
    ~gb_emulator_opts();

    void print_usage();
    void print_help();
    bool parse_opts();

private:
    using opt_handler_t = std::function<bool()>;
    using opt_map_t     = std::unordered_map<int, opt_handler_t>;
    using opt_doc_t     = std::array<std::string, 4>;

    int               m_argc;
    char**            m_argv;
    const std::string m_opt_str;
    const opt_doc_t   m_opt_doc;
    const opt_map_t   m_opt_map;

    bool _opt_set_tracing_flag();
    bool _opt_set_debugger_flag();
    bool _opt_print_doc();
};

#endif // GB_EMULATOR_OPTS_H_
