#pragma once
#include <string>

// Owner: Trisha
enum class ProcessState {
    READY,      // waiting in the ready queue
    RUNNING,    // currently on a core
    SLEEPING,   // executed SLEEP, off the core
    FINISHED    // all instructions done
};

// "READY", "RUNNING", "SLEEPING", "FINISHED"
std::string processStateToString(ProcessState state);
