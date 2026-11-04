#include "console/Banner.h"

#include <iostream>

// TODO(group): fill in full names and the date of your last edit.
static const char* const kDevelopers[] = {
    "Ryan",
    "Trisha",
    "Karl",
    "Alain",
};
static const char* const kLastUpdated = "11-04-2026";

void Banner::print() {
    std::cout << "  ____ ____   ___  ____  _____ ______   __\n"
                 " / ___/ ___| / _ \\|  _ \\| ____/ ___\\ \\ / /\n"
                 "| |   \\___ \\| | | | |_) |  _| \\___ \\\\ V / \n"
                 "| |___ ___) | |_| |  __/| |___ ___) || |  \n"
                 " \\____|____/ \\___/|_|   |_____|____/ |_|  \n\n";
    std::cout << "Welcome to CSOPESY Emulator!\n\n";
    std::cout << "Developers:\n";
    for (const char* d : kDevelopers) std::cout << d << "\n";
    std::cout << "\nLast updated: " << kLastUpdated << "\n\n";
    std::cout << "Type 'initialize' to begin, or 'exit' to quit.\n\n";
}
