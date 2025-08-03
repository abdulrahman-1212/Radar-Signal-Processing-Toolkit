#include "src/radar_cli.h"

int main(int argc, char* argv[]) {
    RadarToolkit::RadarCLI cli;
    if (argc > 2 && std::string(argv[1]) == "--batch") {
        cli.runBatch(argv[2]);
    } else {
        cli.run();
    }
    return 0;
}