#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "process/Process.h"

// Owner: Trisha
// Registry of EVERY process (ready, running, sleeping, finished).
// Locks internally, safe from both threads.
class ProcessTable {
public:
    ProcessTable();

    // Returns false (and adds nothing) if the name is already used.
    bool add(std::shared_ptr<Process> process);

    // nullptr if not found.
    std::shared_ptr<Process> find(const std::string& name) const;

    // Copy of the list, in creation order.
    std::vector<std::shared_ptr<Process>> getAll() const;

    // Hands out 1, 2, 3, ... each call.
    int nextId();

private:
    mutable std::mutex mutex;
    std::vector<std::shared_ptr<Process>> processes;
    int idCounter;
};
