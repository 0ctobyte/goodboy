#ifndef _GB_EMULATOR_OPTS_H_
#define _GB_EMULATOR_OPTS_H_

#include <string>
#include <map>

class gb_emulator_opts {
public:
    std::string m_program_name;
    std::string m_rom_filename;
    bool        m_debugger;

    gb_emulator_opts(int argc, char **argv);
    ~gb_emulator_opts();

    void print_usage();
    void print_help();
    bool parse_opts();

private:
    typedef bool (gb_emulator_opts::*opt_handler_t)();

    int                            _m_argc;
    char                         **_m_argv;
    std::string                    _m_opt_str = "hd";
    std::string                    _m_opt_doc[3] = {
        "-h          : Print this help and exit",
        "-d          : Run in debugger mode",
        "rom_file    : Gameboy program to run on the emulator"
    };
    std::map<int, opt_handler_t>   _m_opt_map = {
        {'d', &gb_emulator_opts::_opt_set_debugger_flag},
        {'h', &gb_emulator_opts::_opt_print_doc}
    };

    bool _opt_set_debugger_flag();
    bool _opt_print_doc();
};

#endif // _GB_EMULATOR_OPTS_H_
