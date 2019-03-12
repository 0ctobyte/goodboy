#include "gb_logger.h"
#include "gb_emulator_opts.h"
#include "gb_debugger.h"

int main(int argc, char **argv) {
    gb_emulator_opts options (argc, argv);

    if (!options.parse_opts()) return EXIT_FAILURE;

    auto load_rom = [&options](auto& gbm) -> void {
        try {
            gbm.load_rom(options.m_rom_filename);
        } catch (std::runtime_error rerr) {
            GB_LOGGER(GB_LOG_FATAL) << rerr.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    };

    if (options.m_debugger) {
        gb_debugger gbm;

        // Need to setup stream with gb_logger again since gb_emulator_debugger modifies the
        // the reference of the underlying streambuf
        gb_logger::instance().set_stream(std::cout);
        gb_logger::instance().enable_tracing(true);

        load_rom(gbm);
        gbm.go();
    } else {
        gb_emulator gbm;
        gb_logger::instance().enable_tracing(options.m_tracing);

        load_rom(gbm);
        gbm.go();
    }

    return EXIT_SUCCESS;
}
