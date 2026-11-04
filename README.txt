csopesy-emulator/
├── README.txt                  # REQUIRED: member names, how to build/run, entry file (src/main.cpp)
├── CMakeLists.txt              # or Makefile
├── config.txt                  # sample/default config, read by "initialize"
├── .gitignore                  # build/, *.o, csopesy-log.txt, IDE folders
│
├── src/
│   ├── main.cpp                # ENTRY POINT: starts the console loop
│   │
│   ├── console/                # Console UI + command interpreter
│   │   ├── Console.h/.cpp          # main menu loop, prompt "root:\>", clear screen
│   │   ├── Banner.h/.cpp           # ASCII header, developers, last updated
│   │   ├── CommandParser.h/.cpp    # tokenizes input, recognizes commands
│   │   ├── CommandHandler.h/.cpp   # dispatches: initialize, exit, screen,
│   │   │                           #   scheduler-start/stop, report-util
│   │   └── ProcessScreen.h/.cpp    # per-process screen: process-smi, exit
│   │
│   ├── config/
│   │   └── Config.h/.cpp           # parses config.txt, validates ranges
│   │                               #   (num-cpu, scheduler, quantum-cycles,
│   │                               #    batch-process-freq, min-ins, max-ins,
│   │                               #    delay-per-exec)
│   │
│   ├── process/                # Process representation
│   │   ├── Process.h/.cpp          # name, ID, state, core, instruction pointer,
│   │   │                           #   logs, sleep counter, timestamps
│   │   ├── ProcessState.h          # enum: READY, RUNNING, SLEEPING, FINISHED
│   │   ├── ProcessTable.h/.cpp     # registry: lookup by name (screen -r/-s)
│   │   ├── ProcessGenerator.h/.cpp # scheduler-start: creates p01, p02... with
│   │   │                           #   random instructions every X ticks
│   │   └── SymbolTable.h/.cpp      # per-process variables (uint16, clamped 0–65535)
│   │
│   ├── instructions/           # Barebones instructions
│   │   ├── Instruction.h/.cpp           # abstract base: execute(Process&)
│   │   ├── PrintInstruction.h/.cpp
│   │   ├── DeclareInstruction.h/.cpp
│   │   ├── AddInstruction.h/.cpp
│   │   ├── SubtractInstruction.h/.cpp
│   │   ├── SleepInstruction.h/.cpp
│   │   ├── ForInstruction.h/.cpp   # nested up to 3 levels
│   │   └── InstructionFactory.h/.cpp # random instruction generation
│   │   ├── Operand.h/.cpp   # nested up to 3 levels
│   │
│   ├── scheduler/              # Scheduler implementation
│   │   ├── Scheduler.h/.cpp        # abstract base + ready queue
│   │   ├── FCFSScheduler.h/.cpp
│   │   ├── RRScheduler.h/.cpp      # quantum-cycles preemption
│   │   ├── CPUCore.h/.cpp          # one worker per core, runs assigned process
│   │   └── CPUClock.h/.cpp         # global CPU tick counter, delay-per-exec
│   │
│   ├── report/
│   │   └── ReportGenerator.h/.cpp  # builds screen -ls text; report-util writes
│   │                               #   it to csopesy-log.txt
│   │
│   └── utils/
│       ├── Timestamp.h/.cpp        # (MM/DD/YYYY HH:MM:SSAM) formatting
│       ├── Logger.h/.cpp           # per-process print logs with core + time
│       └── StringUtils.h/.cpp      # trim, split, lowercase
│
├── tests/                      # Test cases and config variants
│   ├── configs/
│   │   ├── config_fcfs.txt
│   │   ├── config_rr.txt
│   │   ├── config_1cpu.txt
│   │   ├── config_128cpu.txt
│   │   └── config_delay.txt
│   ├── test_config.cpp
│   ├── test_instructions.cpp       # clamping, nested FOR, auto-declare
│   ├── test_scheduler.cpp
│   └── test_commands.cpp           # "initialize first" rule, bad inputs
│
│
└── build/                      # compiled output (gitignored)
