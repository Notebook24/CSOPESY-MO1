#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "Process.h"

class ProcessTable {
public:
    bool add(const std::shared_ptr<Process>& process);

    std::shared_ptr<Process> find(const std::string& name) const;

    bool exists(const std::string& name) const;

    std::vector<std::shared_ptr<Process>> all() const;

    size_t size() const;

private:
    mutable std::mutex mutex_;
    std::map<std::string, std::shared_ptr<Process>> byName_;
    std::vector<std::shared_ptr<Process>> ordered_;
};