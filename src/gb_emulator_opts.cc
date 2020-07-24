/*
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <stdexcept>

#include <unistd.h>

#include "gb_emulator_opts.h"

#define OPT_STR_INIT "hdt"
#define OPT_DOC_INIT \
{\
    "-h          : Print this help and exit",\
    "-d          : Run in debugger mode",\
    "-t          : Enable tracing",\
    "rom_file    : Gameboy program to run on the emulator"\
}
#define OPT_MAP_INIT \
{\
    {'h', std::bind(&gb_emulator_opts::_opt_print_doc, this)},\
    {'d', std::bind(&gb_emulator_opts::_opt_set_debugger_flag, this)},\
    {'t', std::bind(&gb_emulator_opts::_opt_set_tracing_flag, this)}\
}

gb_emulator_opts::gb_emulator_opts(int argc, char **argv)
    : m_program_name(argv[0]), m_argc(argc), m_argv(argv), m_opt_str(OPT_STR_INIT), m_opt_doc(OPT_DOC_INIT), m_opt_map(OPT_MAP_INIT) {
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

    for (const std::string& doc : m_opt_doc) {
        std::cout << doc << std::endl;
    }

    return false;
}

bool gb_emulator_opts::_opt_set_debugger_flag() {
    m_debugger = true;
    return true;
}

bool gb_emulator_opts::_opt_set_tracing_flag() {
    m_tracing = true;
    return true;
}

bool gb_emulator_opts::parse_opts() {
    for (int c = 0; (c = getopt(m_argc, m_argv, m_opt_str.c_str())) != -1; ) {
        try {
            opt_handler_t opt_handler = m_opt_map.at(c);
            bool ok = opt_handler();
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
