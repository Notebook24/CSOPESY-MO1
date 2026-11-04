# CSOPESY MO1: shared contracts

Copy `src/*` into the repo. These headers are the agreement between all four
members. If you want to change one, tell the group first.

## Conventions
- Includes are rooted at `src/`: `#include "process/Process.h"`.
  CMake needs `include_directories(src)`.
- PascalCase file names (`Config.h`, `Console.h`, `Banner.h`). On macOS use a
  two-step rename: `git mv config.h tmp.h && git mv tmp.h Config.h`.
- `#pragma once` in every header.
- Config key is `delays-per-exec` (matches the spec and config.txt).
- Report file is `csopesy-log.txt`. Commands are `screen -ls` and `report-util`.

## Who calls whom
| Caller | Calls |
|---|---|
| CommandHandler (Ryan) | Config::loadFromFile, ProcessTable::find, ProcessGenerator::createProcess/start/stop, Scheduler::start/stop, ReportGenerator::buildReport/saveToFile |
| ProcessScreen (Ryan) | Process::getName/getId/getLogs/getInstructionPointer/getTotalInstructions/isFinished |
| ProcessGenerator (Trisha) | InstructionFactory::generateProgram, ProcessTable::add/nextId, Scheduler::addProcess |
| Scheduler (Karl) | Process::executeNextInstruction/setState/setCoreId/getSleepTicks/decrementSleepTicks/isFinished |
| Process (Trisha) | Logger::formatPrintLog, Timestamp::now |
| Instructions (Alain) | Process::getSymbols/addLog/setSleepTicks |
| ReportGenerator (Ryan) | ProcessTable::getAll, Process getters, Scheduler::getBusyCores etc. |

## Timing rules (Karl)
- One scheduler thread. Cores are plain objects stepped once per tick.
- One tick = one instruction when delays-per-exec = 0.
- delays-per-exec = N: run one instruction, then keep the core busy N more ticks.
- RR quantum counts ticks the process holds a core (delay ticks included).
- SLEEP(X): process leaves the core, SLEEPING for X ticks, then goes back to
  the END of the ready queue.
- scheduler-stop only stops generating. The scheduler keeps running.

## Wiring done in CommandHandler::initialize (Ryan)
    Config config; config.loadFromFile("config.txt", error);
    clock     = new CPUClock();
    table     = new ProcessTable();
    scheduler = (config.scheduler == "fcfs")
                  ? (Scheduler*) new FCFSScheduler(config.numCpu, config.delaysPerExec, *clock)
                  : (Scheduler*) new RRScheduler(config.numCpu, config.delaysPerExec, *clock, config.quantumCycles);
    generator = new ProcessGenerator(*table, *scheduler, config.batchProcessFreq, config.minIns, config.maxIns);
    scheduler->setTickHook([this](uint64_t tick) { generator->onTick(tick); });
    scheduler->start();
    // On exit: scheduler->stop() FIRST, then delete generator, scheduler, table, clock.
    // Typing "initialize" twice: print "Already initialized." and do nothing.
