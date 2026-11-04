#pragma once
#include <memory>
#include <string>

#include "config/Config.h"
#include "console/CommandParser.h"

class CPUClock;
class ProcessTable;
class Scheduler;
class ProcessGenerator;

// Dispatches main-menu commands. Owns the emulator objects created by "initialize".
class CommandHandler {
public:
    CommandHandler();
    ~CommandHandler();  // defined in .cpp (needs the full types)

    void execute(const std::string& line);
    bool shouldExit() const { return exit_; }

private:
    void handleInitialize();
    void handleScreen(const ParsedCommand& cmd);
    void handleSchedulerStart();
    void handleSchedulerStop();
    void handleReportUtil();
    void shutdown();  // scheduler->stop() FIRST, then tear down in reverse order

    bool initialized_ = false;
    bool generating_  = false;
    bool exit_        = false;

    Config config_;
    std::unique_ptr<CPUClock>         clock_;
    std::unique_ptr<ProcessTable>     table_;
    std::unique_ptr<Scheduler>        scheduler_;
    std::unique_ptr<ProcessGenerator> generator_;
};
