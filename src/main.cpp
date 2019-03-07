#include "gb_emulator_opts.h"
#include "gb_emulator_debugger.h"

int main(int argc, char **argv) {
    gb_emulator_opts options (argc, argv);

    if (!options.parse_opts()) return EXIT_FAILURE;

    if (options.m_debugger) {
        gb_emulator_debugger gbm (options.m_rom_filename);
        gbm.go();
    } else {
        gb_emulator gbm (options.m_rom_filename);
        gbm.tracing(options.m_tracing);
        gbm.go();
    }

    return EXIT_SUCCESS;
}
