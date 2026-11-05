CSOPESY MO1 - Process Scheduler and CLI
=========================================

Section S01, Group 4

Members:
  Trisha Mae Bendol
  Karl Kristoffer Camato
  Ryan James Malapitan
  Alain Zuriel Marcos

The main function is in src/main.cpp.


How to build
------------
You need a C++17 compiler and CMake. From the project folder:

  cmake -S . -B build
  cmake --build build

This makes an executable called csopesy inside build/. CMake also copies
config.txt into build/. If you change config.txt in the main folder, run the
cmake -S . -B build line again, or just edit the copy inside build/.


How to run
----------
The program looks for config.txt in the folder you start it from, so run it
from build/:

  cd build
  csopesy.exe        (on Linux/Mac: ./csopesy)

If you built with Visual Studio the exe might be in build/Debug instead. Make
sure config.txt is in the same folder as the exe.

You don't have to recompile to change settings. Edit config.txt, restart the
program, and type initialize.


config.txt
----------
Each line is a parameter and a value. All seven are needed. Quotes around the
scheduler name are optional.

  num-cpu 4               number of cores (1 to 128)
  scheduler "rr"          fcfs or rr
  quantum-cycles 5        time slice for rr
  batch-process-freq 1    a new process is made every this many ticks
  min-ins 1000            fewest instructions a process can have
  max-ins 2000            most instructions a process can have
  delays-per-exec 0       extra ticks a core waits after each instruction

If the file is missing or has a bad value, initialize shows an error and
nothing else will work until it succeeds.


Commands
--------
Only initialize and exit work until initialize has been run. Commands are not
case sensitive.

  initialize         reads config.txt and starts the scheduler
  scheduler-start    starts making dummy processes (p01, p02, ...)
  scheduler-stop     stops making new ones. Existing processes keep running.
  screen -s <name>   makes a new process and opens its screen
  screen -r <name>   opens a running process. If it doesn't exist or already
                     finished it says "Process <name> not found."
  screen -ls         shows CPU utilization, cores used, cores available, and
                     the running and finished processes
  report-util        saves the same thing as screen -ls into csopesy-log.txt
                     (in the folder you started the program from)
  exit               closes the program

Inside a process screen:

  process-smi        shows the process name, ID, its print logs, and how far
                     along it is. Shows "Finished!" if it's done.
  exit               goes back to the main menu


Instructions
------------
Processes get random instructions when they are created. The user doesn't
type them. The number of instructions is random between min-ins and max-ins.

  PRINT, DECLARE, ADD, SUBTRACT, SLEEP, and FOR (nested up to 3 deep)

Variables are uint16, so values stay between 0 and 65535. A variable that is
used before it is declared starts at 0. FOR loops are unrolled when the
process is made, so the instruction count in screen -ls is the unrolled count.


How the scheduler works
-----------------------
There is one scheduler thread. Each tick it moves every core forward one step.

- fcfs: processes run in the order they arrive until they finish or sleep.
- rr: a process is kicked off after quantum-cycles ticks and goes to the back
  of the queue. Delay ticks count toward the quantum.
- SLEEP(x): the process leaves the core, waits x ticks, then goes to the back
  of the queue.
- delays-per-exec: after each instruction the core stays busy for that many
  extra ticks.

The process list and ready queue are locked since the console, the process
generator, and the cores all use them at the same time.


Files
-----
  CMakeLists.txt, config.txt, CONTRACTS.md
  src/main.cpp       entry point
  src/console        main menu and process screen
  src/config         reads config.txt
  src/process        process, variables, process table, process generator
  src/instructions   the instructions and the random program maker
  src/scheduler      fcfs, rr, cores, clock
  src/report         builds the screen -ls / report-util text
  src/utils          timestamps, string helpers
  tests              small test programs (not needed to run the emulator)