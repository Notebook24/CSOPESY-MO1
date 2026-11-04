#pragma once
#include <string>

// Owner: Alain
class Process;

// Base class for PRINT, DECLARE, ADD, SUBTRACT, SLEEP.
//
// RULES:
//  - execute() does ONE instruction and returns. It never loops or waits.
//  - Variables are read/written through process.getSymbols().
//  - PRINT calls process.addLog(text).
//  - SLEEP calls process.setSleepTicks(x). It does NOT change the state.
//    The scheduler sees sleepTicks > 0 and takes the process off the core.
class Instruction {
public:
    virtual ~Instruction() {}
    virtual void execute(Process& process) = 0;
    virtual std::string toString() const = 0;
};
