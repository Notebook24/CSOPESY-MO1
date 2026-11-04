cmake_minimum_required(VERSION 3.10)
project(csopesy_emulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)


find_package(Threads REQUIRED)

add_executable(test_scheduler
    tests/test_scheduler.cpp
    tests/stubs/Process.cpp          # To be removed once Trisha's real Process.cpp exists
    src/scheduler/CPUClock.cpp
    src/scheduler/CPUCore.cpp
    src/scheduler/Scheduler.cpp
    src/scheduler/FCFSScheduler.cpp
    src/scheduler/RRScheduler.cpp)

target_link_libraries(test_scheduler Threads::Threads)