// TEST STUB. Stands in for Trisha's real src/process/Process.cpp
// so the scheduler tests can build before her code exists.
// Remove this file from the test build once the real one is ready.
#include "process/Process.h"

Process::Process(int id, const std::string& name,
                 const std::vector<std::shared_ptr<Instruction>>& instructions)
    : id(id),
      name(name),
      state(ProcessState::READY),
      coreId(-1),
      instructionPointer(0),
      instructions(instructions),
      sleepTicks(0) {
}

int Process::getId() const { std::lock_guard<std::mutex> lock(mutex); return id; }
std::string Process::getName() const { std::lock_guard<std::mutex> lock(mutex); return name; }
std::string Process::getCreatedTime() const { return createdTime; }
std::string Process::getFinishedTime() const { std::lock_guard<std::mutex> lock(mutex); return finishedTime; }

ProcessState Process::getState() const { std::lock_guard<std::mutex> lock(mutex); return state; }
void Process::setState(ProcessState newState) { std::lock_guard<std::mutex> lock(mutex); state = newState; }
int Process::getCoreId() const { std::lock_guard<std::mutex> lock(mutex); return coreId; }
void Process::setCoreId(int newCoreId) { std::lock_guard<std::mutex> lock(mutex); coreId = newCoreId; }

size_t Process::getInstructionPointer() const { std::lock_guard<std::mutex> lock(mutex); return instructionPointer; }
size_t Process::getTotalInstructions() const { std::lock_guard<std::mutex> lock(mutex); return instructions.size(); }
bool Process::isFinished() const { std::lock_guard<std::mutex> lock(mutex); return state == ProcessState::FINISHED; }

void Process::executeNextInstruction() {
    std::shared_ptr<Instruction> next;
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (instructionPointer >= instructions.size()) {
            return;
        }
        next = instructions[instructionPointer];
    }

    // Mutex is NOT held here, so the instruction may call addLog() etc.
    next->execute(*this);

    bool wasLast = false;
    {
        std::lock_guard<std::mutex> lock(mutex);
        instructionPointer++;
        wasLast = (instructionPointer >= instructions.size());
    }
    if (wasLast) {
        markFinished();
    }
}

void Process::markFinished() {
    std::lock_guard<std::mutex> lock(mutex);
    state = ProcessState::FINISHED;
    coreId = -1;
    finishedTime = "done";
}

void Process::addLog(const std::string& message) { std::lock_guard<std::mutex> lock(mutex); logs.push_back(message); }
std::vector<std::string> Process::getLogs() const { std::lock_guard<std::mutex> lock(mutex); return logs; }

SymbolTable& Process::getSymbols() { return symbols; }

void Process::setSleepTicks(int ticks) { std::lock_guard<std::mutex> lock(mutex); sleepTicks = ticks; }
int Process::getSleepTicks() const { std::lock_guard<std::mutex> lock(mutex); return sleepTicks; }
void Process::decrementSleepTicks() {
    std::lock_guard<std::mutex> lock(mutex);
    if (sleepTicks > 0) {
        sleepTicks--;
    }
}
