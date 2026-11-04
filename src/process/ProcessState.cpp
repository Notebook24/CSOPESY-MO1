#include "ProcessState.h"

std::string processStateToString(ProcessState s) {
    switch (s) {
        case ProcessState:: READY:
            return "READY";
        case ProcessState:: RUNNING:
            return "RUNNING";
        case ProcessState:: SLEEPING:
            return "SLEEPING";
        case ProcessState:: FINISHED:
            return "FINISHED";
    }
    return "UNKNOWN";
}