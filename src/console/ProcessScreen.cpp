#include "console/ProcessScreen.h"

#include <iostream>
#include <string>

#include "console/CommandParser.h"
#include "console/Terminal.h"
#include "process/Process.h"

void ProcessScreen::printSmi() const {
    std::cout << "Process name: " << process_.name() << "\n";
    std::cout << "ID: " << process_.id() << "\n";
    std::cout << "Logs:\n";
    for (const auto& entry : process_.logs()) {
        std::cout << entry.timestamp << " Core:" << entry.coreId
                  << " \"" << entry.message << "\"\n";
    }
    std::cout << "\n";
    if (process_.isFinished()) {
        std::cout << "Finished!\n";
    } else {
        std::cout << "Current instruction line: " << process_.currentLine() << "\n";
        std::cout << "Lines of code: " << process_.totalLines() << "\n";
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
