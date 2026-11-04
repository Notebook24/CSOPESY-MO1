#pragma once
#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

// Per-process print log. Each entry is stored pre-formatted as:
//   (08/06/2024 09:15:22AM) Core:0 "Hello world from screen_01!"
// Thread-safe: CPU-core threads write while the console thread reads (process-smi).
// Not copyable (owns a mutex) -> store it as a member of Process, or hold Process by pointer.
class Logger {
public:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(const std::string& message, int coreId);

    // Snapshot copy of all entries (safe to iterate without holding the lock).
    std::vector<std::string> getEntries() const;

    std::size_t size() const;
    void clear();

private:
    mutable std::mutex mutex_;
    std::vector<std::string> entries_;
};