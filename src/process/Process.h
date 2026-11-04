#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "instructions/Instruction.h"
#include "process/ProcessState.h"
#include "process/SymbolTable.h"

// Owner: Trisha
// Shared by the console thread and the scheduler thread, so every getter and
// setter locks 'mutex' and getters return COPIES (never references), except
// getSymbols().
class Process {
public:
    Process(int id, const std::string& name,
            const std::vector<std::shared_ptr<Instruction>>& instructions);

    // ----- identity -----
    int getId() const;
    std::string getName() const;
    std::string getCreatedTime() const;    // set in constructor
    std::string getFinishedTime() const;   // "" until finished

    // ----- state (the scheduler writes these) -----
    ProcessState getState() const;
    void setState(ProcessState state);
    int getCoreId() const;                 // -1 when not on a core
    void setCoreId(int coreId);

    // ----- progress -----
    size_t getInstructionPointer() const;  // how many instructions are done
    size_t getTotalInstructions() const;
    bool isFinished() const;

    // Runs the instruction at the pointer, then moves the pointer forward.
    // If that was the last one, calls markFinished().
    // IMPORTANT: do NOT hold the mutex while calling instruction->execute(),
    // because execute() calls addLog() which locks the same mutex.
    // Copy the shared_ptr under the lock, release, execute, lock again, advance.
    void executeNextInstruction();

    // Sets state FINISHED, coreId -1, finished time.
    void markFinished();

    // ----- output (PRINT) -----
    // Stores Logger::formatPrintLog(coreId, message) in the log list.
    void addLog(const std::string& message);
    std::vector<std::string> getLogs() const;

    // ----- variables -----
    SymbolTable& getSymbols();

    // ----- SLEEP -----
    void setSleepTicks(int ticks);         // called by SleepInstruction (0..255)
    int getSleepTicks() const;
    void decrementSleepTicks();            // called by the scheduler once per tick

private:
    mutable std::mutex mutex;
    int id;
    std::string name;
    ProcessState state;
    int coreId;
    size_t instructionPointer;
    std::vector<std::shared_ptr<Instruction>> instructions;
    std::vector<std::string> logs;
    SymbolTable symbols;
    int sleepTicks;
    std::string createdTime;
    std::string finishedTime;
};
