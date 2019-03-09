#include "gb_logger.h"
#include "gb_emulator_opts.h"
#include "gb_emulator_debugger.h"

int main(int argc, char **argv) {
    gb_emulator_opts options (argc, argv);

    if (!options.parse_opts()) return EXIT_FAILURE;

    if (options.m_debugger) {
        gb_emulator_debugger gbm;
        gb_logger::instance().set_level(GB_LOG_DEBUG);
        if (!gbm.load_rom(options.m_rom_filename)) {
            GB_LOGGER(GB_LOG_FATAL) << "Not a valid ROM file: " << options.m_rom_filename << std::endl;
            return EXIT_FAILURE;
        }
        gbm.go();
    } else {
        gb_emulator gbm;
        if (options.m_tracing) gb_logger::instance().set_level(GB_LOG_DEBUG);
        if (!gbm.load_rom(options.m_rom_filename)) {
            GB_LOGGER(GB_LOG_FATAL) << "Not a valid ROM file: " << options.m_rom_filename << std::endl;
            return EXIT_FAILURE;
        }
        gbm.go();
    }

    return EXIT_SUCCESS;
}
