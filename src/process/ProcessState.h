#pragma once

#include <string>

enum class ProcessState {READY, RUNNING, SLEEPING, FINISHED};

std::string processStateToString(ProcessState S);