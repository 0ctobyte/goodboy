#include "gb_logger.h"
#include "gb_emulator_opts.h"
#include "gb_debugger.h"

int main(int argc, char **argv) {
    gb_emulator_opts options (argc, argv);

    if (!options.parse_opts()) return EXIT_FAILURE;

    if (options.m_debugger) {
        gb_debugger gbm;

        // Need to setup stream with gb_logger again since gb_emulator_debugger modifies the
        // the reference of the underlying streambuf
        gb_logger::instance().set_stream(std::cout);
        gb_logger::instance().enable_tracing(true);

        if (!gbm.load_rom(options.m_rom_filename)) {
            GB_LOGGER(GB_LOG_FATAL) << "Not a valid ROM file: " << options.m_rom_filename << std::endl;
            return EXIT_FAILURE;
        }

        gbm.go();
    } else {
        gb_emulator gbm;

        gb_logger::instance().enable_tracing(options.m_tracing);
        if (!gbm.load_rom(options.m_rom_filename)) {
            GB_LOGGER(GB_LOG_FATAL) << "Not a valid ROM file: " << options.m_rom_filename << std::endl;
            return EXIT_FAILURE;
        }

        gbm.go();
    }

    return EXIT_SUCCESS;
}
