#include <iostream>

#include "gb_emulator_opts.h"
#include "gb_emulator_debugger.h"

int main(int argc, char **argv) {
    gb_emulator_opts options (argc, argv);

    if (!options.parse_opts()) return EXIT_FAILURE;

    if (options.m_debugger) {
        gb_emulator_debugger gbm;
        if (!gbm.load_rom(options.m_rom_filename)) {
            std::cout << "Not a valid ROM file: " << options.m_rom_filename << std::endl;
            return EXIT_FAILURE;
        }
        gbm.go();
    } else {
        gb_emulator gbm;
        gbm.tracing(options.m_tracing);
        if (!gbm.load_rom(options.m_rom_filename)) {
            std::cout << "Not a valid ROM file: " << options.m_rom_filename << std::endl;
            return EXIT_FAILURE;
        }
        gbm.go();
    }

    return EXIT_SUCCESS;
}
