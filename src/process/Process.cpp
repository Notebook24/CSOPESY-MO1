#include "Process.h"

#include <ctime>
#include <iomanip>
#include <sstream>

#include "../instructions/Instruction.h"

namespace {
    // Format: (MM/DD/YYYY HH:MM:SSAM)
    std::string nowString() {
        std::time_t t = std::time(nullptr);
        std::tm tm{};
    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif
        std::ostringstream oss;
        oss << "(" << std::put_time(&tm, "%m/%d/%Y %I:%M:%S%p") << ")";
        return oss.str();
    }
}  

Process::Process(uint32_t id, std::string name, std::vector<std::shared_ptr<Instruction>> instructions)
    : id_(id),
      name_(std::move(name)),
      instructions_(std::move(instructions)),
      createdAt_(nowString()) {}

bool Process::executeNext() {
    size_t pc = pc_.load();
    if (pc >= instructions_.size()) {
        markFinished();
        return false;
    }

    instructions_[pc]->execute(*this);   
    pc_.store(pc + 1);

    if (pc + 1 >= instructions_.size()) markFinished();
    return true;
}

void Process::addLog(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    logs_.push_back({nowString(), coreId_.load(), message});
}

std::vector<LogEntry> Process::logs() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return logs_;
}

void Process::sleepFor(uint8_t ticks) {
    if (ticks == 0) return;
    sleepTicks_.store(ticks);
    state_.store(ProcessState::SLEEPING);
}

bool Process::tickSleep() {
    if (state() != ProcessState::SLEEPING) 
        return false;

    uint32_t remaining = sleepTicks_.load();

    if (remaining > 0) 
        sleepTicks_.store(--remaining);

    if (remaining == 0) {
        state_.store(ProcessState::READY);
        return true;
    }

    return false;
}

std::string Process::finishedAt() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return finishedAt_;
}

void Process::markFinished() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!finishedAt_.empty()) return;  
        finishedAt_ = nowString();
    }
    state_.store(ProcessState::FINISHED);
}