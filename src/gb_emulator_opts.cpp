#include <iostream>
#include <stdexcept>

#include <unistd.h>

#include "gb_emulator_opts.h"

gb_emulator_opts::gb_emulator_opts(int argc, char **argv)
    : m_program_name(argv[0]), m_argc(argc), m_argv(argv) {
}

gb_emulator_opts::~gb_emulator_opts() {
}

void gb_emulator_opts::print_usage() {
    std::cout << "usage: " << m_program_name << " [options]" << " rom_file " <<  std::endl;
}

void gb_emulator_opts::print_help() {
    print_usage();
    std::cout << "Try '" << m_program_name << " -h' " << "for more information." << std::endl;
}

bool gb_emulator_opts::_opt_print_doc() {
    print_usage();

    std::cout << std::endl << "Options and arguments:" << std::endl;

    for (unsigned int i = 0; i < (sizeof(m_opt_doc)/sizeof(m_opt_doc[0])); i++) {
        std::cout << m_opt_doc[i] << std::endl;
    }

    return false;
}

bool gb_emulator_opts::_opt_set_debugger_flag() {
    m_debugger = true;
    return true;
}

bool gb_emulator_opts::parse_opts() {
    for (int c = 0; (c = getopt(m_argc, m_argv, m_opt_str.c_str())) != -1; ) {
        try {
            opt_handler_t opt_handler = m_opt_map.at(c);
            bool ok = (this->*opt_handler)();
            if (!ok) return false;
        } catch (std::out_of_range &oor) {
            print_help();
            return false;
        }
    }

    if (optind >= m_argc) {
        std::cout << m_program_name << ": " << "'rom_file' must be specified" << std::endl;
        print_help();
        return false;
    }

    m_rom_filename = std::string(m_argv[optind]);

    return true;
}
