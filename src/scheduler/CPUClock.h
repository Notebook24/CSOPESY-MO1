#pragma once
#include <atomic>
#include <cstdint>

// Owner: Karl
// The global tick counter. Only the scheduler thread calls tick().
class CPUClock {
public:
    CPUClock();
    void tick();                 // +1
    uint64_t getTicks() const;
    void reset();                // back to 0
private:
    std::atomic<uint64_t> ticks;
};
