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