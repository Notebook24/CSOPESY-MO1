#pragma once
<<<<<<< HEAD

#include <atomic>
=======
#include <cstddef>
>>>>>>> origin/main
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
<<<<<<< HEAD

#include "ProcessState.h"
#include "SymbolTable.h"

class Instruction;  

struct LogEntry {
    std::string timestamp;  
    int coreId;
    std::string message;
};

class Process {
public:
    Process(uint32_t id, std::string name, std::vector<std::shared_ptr<Instruction>> instructions);

    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    uint32_t id() const { 
        return id_; 
    }
    const std::string& name() const { 
        return name_; 
    }

    ProcessState state() const { 
        return state_.load(); 
    }

    void setState(ProcessState s) { 
        state_.store(s); 
    }

    int coreId() const { 
        return coreId_.load(); 
    }  
    
    void setCoreId(int core) { 
        coreId_.store(core); 
    }

    bool isFinished() const { 
        return state() == ProcessState::FINISHED; 
    }

    size_t currentLine() const { 
        return pc_.load(); 
    }

    size_t totalLines() const { 
        return instructions_.size(); 
    }

    bool executeNext();

    SymbolTable& symbols() { 
        return symbols_; 
    }

    void addLog(const std::string& message);          
    std::vector<LogEntry> logs() const;               

    void sleepFor(uint8_t ticks);   
    bool tickSleep();               
    bool isSleeping() const { 
        return state() == ProcessState::SLEEPING; 
    }

    const std::string& createdAt() const { 
        return createdAt_; 
    }

    std::string finishedAt() const;   

private:
    void markFinished();

    uint32_t id_;
    std::string name_;
    std::vector<std::shared_ptr<Instruction>> instructions_;
    SymbolTable symbols_;

    std::atomic<ProcessState> state_{ProcessState::READY};
    std::atomic<int> coreId_{-1};
    std::atomic<size_t> pc_{0};
    std::atomic<uint32_t> sleepTicks_{0};

    mutable std::mutex mutex_;   
    std::vector<LogEntry> logs_;
    std::string createdAt_;
    std::string finishedAt_;
};
=======
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
>>>>>>> origin/main
