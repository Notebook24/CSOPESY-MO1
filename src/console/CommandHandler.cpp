#include "console/CommandHandler.h"

#include <iostream>

#include "console/Banner.h"
#include "console/ProcessScreen.h"
#include "console/Terminal.h"
#include "process/Process.h"
#include "process/ProcessGenerator.h"
#include "process/ProcessTable.h"
#include "report/ReportGenerator.h"
#include "scheduler/CPUClock.h"
#include "scheduler/FCFSScheduler.h"
#include "scheduler/RRScheduler.h"
#include "scheduler/Scheduler.h"

static const char* const kConfigPath = "config.txt";
static const char* const kReportPath = "csopesy-log.txt";

CommandHandler::CommandHandler() = default;
CommandHandler::~CommandHandler() { shutdown(); }

void CommandHandler::execute(const std::string& line) {
    ParsedCommand cmd = CommandParser::parse(line);
    if (cmd.empty()) return;

    // Before "initialize", only "initialize" and "exit" are recognized.
    if (!initialized_ && cmd.name != "initialize" && cmd.name != "exit") {
        std::cout << "Please run 'initialize' first.\n";
        return;
    }

    if (cmd.name == "initialize") {
        if (!cmd.args.empty()) { std::cout << "Usage: initialize\n"; return; }
        handleInitialize();
    } else if (cmd.name == "exit") {
        if (!cmd.args.empty()) { std::cout << "Usage: exit\n"; return; }
        shutdown();
        exit_ = true;
    } else if (cmd.name == "screen") {
        handleScreen(cmd);
    } else if (cmd.name == "scheduler-start") {
        if (!cmd.args.empty()) { std::cout << "Usage: scheduler-start\n"; return; }
        handleSchedulerStart();
    } else if (cmd.name == "scheduler-stop") {
        if (!cmd.args.empty()) { std::cout << "Usage: scheduler-stop\n"; return; }
        handleSchedulerStop();
    } else if (cmd.name == "report-util") {
        if (!cmd.args.empty()) { std::cout << "Usage: report-util\n"; return; }
        handleReportUtil();
    } else {
        std::cout << "Unknown command: " << cmd.name << "\n";
    }
}

void CommandHandler::handleInitialize() {
    if (initialized_) {
        std::cout << "Already initialized.\n";
        return;
    }

    Config cfg;
    std::string error;
    if (!loadConfig(kConfigPath, cfg, error)) {
        std::cout << "Initialization failed: " << error << "\n";
        return;
    }
    config_ = cfg;

    clock_ = std::make_unique<CPUClock>();
    table_ = std::make_unique<ProcessTable>();
    if (config_.scheduler == SchedulerType::FCFS) {
        scheduler_.reset(new FCFSScheduler(config_.numCpu, config_.delayPerExec, *clock_));
    } else {
        scheduler_.reset(new RRScheduler(config_.numCpu, config_.delayPerExec, *clock_,
                                         config_.quantumCycles));
    }

    Scheduler* sched = scheduler_.get();
    generator_ = std::make_unique<ProcessGenerator>(config_, *table_,
        [sched](std::shared_ptr<Process> p) { sched->addProcess(p); });

    ProcessGenerator* gen = generator_.get();
    scheduler_->setTickHook([gen](uint64_t tick) { gen->tick(tick); });
    scheduler_->start();

    initialized_ = true;
    std::cout << "Initialized: " << config_.numCpu << " CPU(s), scheduler "
              << schedulerToString(config_.scheduler) << ".\n";
}

void CommandHandler::handleScreen(const ParsedCommand& cmd) {
    static const char* const usage =
        "Usage: screen -s <process name> | screen -r <process name> | screen -ls\n";

    if (cmd.args.empty()) { std::cout << usage; return; }
    const std::string flag = CommandParser::lower(cmd.args[0]);

    if (flag == "-ls") {
        if (cmd.args.size() != 1) { std::cout << usage; return; }
        std::cout << ReportGenerator::buildReport(*table_, *scheduler_);
        return;
    }

    if (flag != "-s" && flag != "-r") { std::cout << usage; return; }
    if (cmd.args.size() != 2) {
        std::cout << "Usage: screen " << flag << " <process name>\n";
        return;
    }
    const std::string& name = cmd.args[1];  // process names are case-sensitive

    std::shared_ptr<Process> proc = table_->find(name);
    if (flag == "-s") {
        if (proc) { std::cout << "Process " << name << " already exists.\n"; return; }
        proc = generator_->createProcess(name);
        if (!proc) { std::cout << "Could not create process " << name << ".\n"; return; }
    } else {
        if (!proc || proc->isFinished()) {
            std::cout << "Process " << name << " not found.\n";
            return;
        }
    }

    ProcessScreen(*proc).run();
    Terminal::clear();
    Banner::print();
}

void CommandHandler::handleSchedulerStart() {
    if (generating_) { std::cout << "Scheduler is already generating processes.\n"; return; }
    generator_->start();
    generating_ = true;
    std::cout << "Scheduler started: generating processes.\n";
}

void CommandHandler::handleSchedulerStop() {
    if (!generating_) { std::cout << "Scheduler is not generating processes.\n"; return; }
    generator_->stop();
    generating_ = false;
    std::cout << "Scheduler stopped: no longer generating processes.\n";
}

void CommandHandler::handleReportUtil() {
    const std::string report = ReportGenerator::buildReport(*table_, *scheduler_);
    if (ReportGenerator::saveToFile(kReportPath, report)) {
        std::cout << "Report generated at " << kReportPath << "!\n";
    } else {
        std::cout << "Could not write " << kReportPath << ".\n";
    }
}

void CommandHandler::shutdown() {
    if (!initialized_) return;
    if (scheduler_) scheduler_->stop();  // stop the thread before anything it uses dies
    generator_.reset();
    scheduler_.reset();
    table_.reset();
    clock_.reset();
    initialized_ = false;
    generating_  = false;
}
