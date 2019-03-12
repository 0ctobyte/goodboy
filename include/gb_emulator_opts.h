#ifndef GB_EMULATOR_OPTS_H_
#define GB_EMULATOR_OPTS_H_

#include <string>
#include <unordered_map>

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
    using opt_handler_t = bool (gb_emulator_opts::*)();
    using opt_map_t     = std::unordered_map<int, opt_handler_t>;

    int                            m_argc;
    char**                         m_argv;
    std::string                    m_opt_str = "hdt";
    std::string                    m_opt_doc[4] = {
        "-h          : Print this help and exit",
        "-d          : Run in debugger mode",
        "-t          : Enable tracing",
        "rom_file    : Gameboy program to run on the emulator"
    };
    opt_map_t                      m_opt_map = {
        {'h', &gb_emulator_opts::_opt_print_doc},
        {'d', &gb_emulator_opts::_opt_set_debugger_flag},
        {'t', &gb_emulator_opts::_opt_set_tracing_flag}
    };

    bool _opt_set_tracing_flag();
    bool _opt_set_debugger_flag();
    bool _opt_print_doc();
};

#endif // GB_EMULATOR_OPTS_H_
