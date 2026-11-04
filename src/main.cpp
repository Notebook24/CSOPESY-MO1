// CSOPESY MO1 - Process Scheduler and CLI
// ENTRY POINT. Build/run instructions are in README.txt.
//
// This is a working template: it compiles with only the utils/ files present.
// Replace each TODO with calls into your real modules, then (optionally) move the
// loop into console/Console.cpp and reduce main() to `Console().run();`.

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "utils/StringUtils.h"

// ---- TODO: uncomment as these modules get written --------------------------
// #include "config/Config.h"
// #include "console/Banner.h"
// #include "console/ProcessScreen.h"
// #include "process/ProcessTable.h"
// #include "process/ProcessGenerator.h"
// #include "scheduler/Scheduler.h"
// #include "report/ReportGenerator.h"
// -----------------------------------------------------------------------------

namespace {

const std::string PROMPT = "root:\\> ";

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

// TODO: replace with Banner::print() (ASCII art, developers, last updated).
void printBanner() {
    std::cout << "CSOPESY\n"
              << "----------------------------------------\n"
              << "Welcome to CSOPESY Emulator!\n\n"
              << "Developers:\n"
              << "<Member 1>, <Member 2>, <Member 3>, <Member 4>\n\n"
              << "Last updated: <MM-DD-YYYY>\n"
              << "----------------------------------------\n";
}

// Per-process screen opened by "screen -s <name>" / "screen -r <name>".
// Supports: process-smi, exit.
void runProcessScreen(const std::string& processName) {
    clearScreen();
    // TODO: print process-smi info once (same as the "process-smi" command below).
    std::string line;
    while (true) {
        std::cout << PROMPT;
        if (!std::getline(std::cin, line)) return;
        line = StringUtils::toLower(StringUtils::trim(line));

        if (line == "process-smi") {
            // TODO: look up the process in ProcessTable and print:
            //   Process name: <name>
            //   ID: <id>
            //   Logs: ...            (from the process's Logger)
            //   Current instruction line: <ip>
            //   Lines of code: <total>
            //   or "Finished!" after the logs when the process is done.
            std::cout << "Process name: " << processName << "\n";
            std::cout << "[TODO] process-smi not implemented yet\n";
        } else if (line == "exit") {
            clearScreen();
            printBanner();
            return;
        } else if (!line.empty()) {
            std::cout << "Unknown command. Available: process-smi, exit\n";
        }
    }
}

// Returns true while the console should keep running.
bool handleCommand(const std::vector<std::string>& tokens, bool& initialized) {
    const std::string cmd = StringUtils::toLower(tokens[0]);

    if (cmd == "exit") {
        // TODO: stop scheduler/generator threads and join them before returning.
        return false;
    }

    // Gate: nothing but "initialize" and "exit" works before initialization.
    if (!initialized && cmd != "initialize") {
        std::cout << "Please run \"initialize\" first.\n";
        return true;
    }

    if (cmd == "initialize") {
        // TODO: Config cfg; if (!cfg.load("config.txt")) { print error; return true; }
        //       build the scheduler (fcfs / rr), spawn CPU cores, start the CPU clock.
        initialized = true;
        std::cout << "[TODO] config.txt loading + scheduler startup not implemented yet\n";
        std::cout << "Initialized.\n";

    } else if (cmd == "screen") {
        if (tokens.size() == 3 && tokens[1] == "-s") {
            // TODO: create the process (random program via InstructionFactory), add to
            //       ProcessTable + ready queue; reject duplicate names.
            runProcessScreen(tokens[2]);
        } else if (tokens.size() == 3 && tokens[1] == "-r") {
            // TODO: if ProcessTable has a RUNNING/READY process named tokens[2]:
            //           runProcessScreen(tokens[2]);
            //       else print: Process <name> not found.
            std::cout << "Process " << tokens[2] << " not found.\n";
        } else if (tokens.size() == 2 && tokens[1] == "-ls") {
            // TODO: std::cout << ReportGenerator::build(...);
            std::cout << "[TODO] screen -ls not implemented yet\n";
        } else {
            std::cout << "Usage: screen -s <name> | screen -r <name> | screen -ls\n";
        }

    } else if (cmd == "scheduler-start") {
        // TODO: ProcessGenerator::start();  (new process every batch-process-freq ticks)
        std::cout << "[TODO] scheduler-start not implemented yet\n";

    } else if (cmd == "scheduler-stop") {
        // TODO: ProcessGenerator::stop();
        std::cout << "[TODO] scheduler-stop not implemented yet\n";

    } else if (cmd == "report-util") {
        // TODO: ReportGenerator::saveToFile("csopesy-log.txt");
        std::cout << "[TODO] report-util not implemented yet\n";

    } else {
        std::cout << "Unknown command: " << tokens[0] << "\n";
    }
    return true;
}

} // namespace

int main() {
    bool initialized = false;

    printBanner();

    std::string line;
    while (true) {
        std::cout << PROMPT;
        if (!std::getline(std::cin, line)) break; // EOF (Ctrl+Z / Ctrl+D)

        std::vector<std::string> tokens = StringUtils::splitWords(StringUtils::trim(line));
        if (tokens.empty()) continue;

        if (!handleCommand(tokens, initialized)) break;
    }
    return 0;
}