# CSOPESY MO1: shared contracts

These headers are the agreement between all four members. If you want to
change one, tell the group first.

Trisha's `Process`, `ProcessTable` and `ProcessGenerator` are the source of
truth for those classes. Use her method names, not the old contract names.

## Conventions
- Includes are rooted at `src/`: `#include "process/Process.h"`.
  CMake needs `include_directories(src)`.
- PascalCase file names (`Config.h`, `Console.h`, `Banner.h`). On macOS use a
  two-step rename: `git mv config.h tmp.h && git mv tmp.h Config.h`.
- Include paths must match the file name's case exactly
  (`config/Config.h`, not `config/config.h`). Linux is case-sensitive.
- `#pragma once` in every header.
- Config key is `delays-per-exec` (matches the spec and config.txt).
- Report file is `csopesy-log.txt`. Commands are `screen -ls` and `report-util`.
- If you get strange link errors after pulling, run `make clean`.

## Who calls whom
| Caller | Calls |
|---|---|
| CommandHandler (Ryan) | Config::loadFromFile, ProcessTable::find, ProcessGenerator::createProcess/start/stop (CHECK), Scheduler::start/stop/addProcess, ReportGenerator::buildReport/saveToFile |
| ProcessScreen (Ryan) | Process::name/id/logs/currentLine/totalLines/state/isFinished (CHECK exact names in Process.h) |
| ProcessGenerator (Trisha) | InstructionFactory::generateProgram, ProcessTable::add, and the callback that calls Scheduler::addProcess |
| Scheduler (Karl) | Process::executeNext/state/setState/setCoreId/isSleeping/tickSleep/isFinished |
| Process (Trisha) | Its own timestamp helper (no Logger or Timestamp calls) |
| Instructions (Alain) | Process::symbols/addLog/sleepFor |
| ReportGenerator (Ryan) | ProcessTable::all, Process getters, Scheduler::getTotalCores/getBusyCores/getAvailableCores/getUtilizationPercent |

## Timing rules (Karl)
- One scheduler thread. Cores are plain objects stepped once per tick.
- One tick = one instruction when delays-per-exec = 0.
- delays-per-exec = N: run one instruction, then keep the core busy N more ticks.
- RR quantum counts ticks the process holds a core (delay ticks included).
- SLEEP(X): the instruction calls `process.sleepFor(X)`. The process leaves the
  core on that same tick, is SLEEPING for X ticks (the scheduler calls
  `tickSleep()` each tick), then goes to the END of the ready queue.
- A finished process frees its core immediately (the scheduler sets coreId to -1).
- scheduler-stop only stops generating. The scheduler keeps running.

## Wiring done in CommandHandler::initialize (Ryan)
    Config config; config.loadFromFile("config.txt", error);   // config must outlive generator
    clock     = new CPUClock();
    table     = new ProcessTable();
    scheduler = (config.scheduler == "fcfs")
                  ? (Scheduler*) new FCFSScheduler(config.numCpu, config.delaysPerExec, *clock)