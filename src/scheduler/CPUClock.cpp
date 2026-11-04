#include "scheduler/CPUClock.h"

CPUClock::CPUClock() : ticks(0) {
}

void CPUClock::tick() {
    ticks++;
}

uint64_t CPUClock::getTicks() const {
    return ticks;
}

void CPUClock::reset() {
    ticks = 0;
}
