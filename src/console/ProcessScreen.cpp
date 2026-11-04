#include "console/ProcessScreen.h"

#include <iostream>
#include <string>

#include "console/CommandParser.h"
#include "console/Terminal.h"
#include "process/Process.h"

void ProcessScreen::printSmi() const {
    std::cout << "Process name: " << process_.getName() << "\n";
    std::cout << "ID: " << process_.getId() << "\n";
    std::cout << "Logs:\n";
    for (const auto& entry : process_.getLogs()) std::cout << entry << "\n";
    std::cout << "\n";
    if (process_.isFinished()) {
        std::cout << "Finished!\n";
    } else {
        std::cout << "Current instruction line: " << process_.getInstructionPointer() << "\n";
        std::cout << "Lines of code: " << process_.getTotalInstructions() << "\n";
    }
    std::cout << "\n";
}

void ProcessScreen::run() {
    Terminal::clear();
    printSmi();

    std::string line;
    while (true) {
        std::cout << "root:\\> " << std::flush;
        if (!std::getline(std::cin, line)) return;  // EOF: drop back to main menu

        ParsedCommand cmd = CommandParser::parse(line);
        if (cmd.empty()) continue;

        if (cmd.name == "exit" && cmd.args.empty()) {
            return;
        } else if (cmd.name == "process-smi" && cmd.args.empty()) {
            printSmi();
        } else {
            std::cout << "Unknown command. Available: process-smi, exit.\n";
        }
    }
}
