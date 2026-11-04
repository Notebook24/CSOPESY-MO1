#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

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