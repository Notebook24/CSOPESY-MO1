cmake_minimum_required(VERSION 3.10)
project(csopesy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# ---------- main emulator (Alain) ----------
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(csopesy ${SOURCES})
target_include_directories(csopesy PRIVATE src)
target_link_libraries(csopesy PRIVATE Threads::Threads)

# copy config.txt next to the executable after configuring
configure_file(${CMAKE_SOURCE_DIR}/config.txt ${CMAKE_BINARY_DIR}/config.txt COPYONLY)

# ---------- scheduler tests (Karl) ----------
add_executable(test_scheduler
    tests/test_scheduler.cpp
    #tests/stubs/Process.cpp          # To be removed once Trisha's real Process.cpp exists
    src/process/Process.cpp
    src/process/SymbolTable.cpp
    src/process/ProcessState.cpp
    src/scheduler/CPUClock.cpp
    src/scheduler/CPUCore.cpp
    src/scheduler/Scheduler.cpp
    src/scheduler/FCFSScheduler.cpp
    src/scheduler/RRScheduler.cpp)
target_include_directories(test_scheduler PRIVATE src)
target_link_libraries(test_scheduler PRIVATE Threads::Threads)

# ---------- more tests (Karl) ----------
file(GLOB INSTRUCTION_SOURCES src/instructions/*.cpp)

add_executable(test_instructions
    tests/test_instructions.cpp
    src/process/Process.cpp
    src/process/SymbolTable.cpp
    src/process/ProcessState.cpp
    ${INSTRUCTION_SOURCES})
target_include_directories(test_instructions PRIVATE src)
target_link_libraries(test_instructions PRIVATE Threads::Threads)

add_executable(test_config
    tests/test_config.cpp
    src/config/Config.cpp)
target_include_directories(test_config PRIVATE src)

# test_commands needs the whole emulator except main.cpp
file(GLOB_RECURSE LIB_SOURCES src/*.cpp)
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)
add_executable(test_commands
    tests/test_commands.cpp
    ${LIB_SOURCES})
target_include_directories(test_commands PRIVATE src)
target_link_libraries(test_commands PRIVATE Threads::Threads)