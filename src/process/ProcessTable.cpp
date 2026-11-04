#include "ProcessTable.h"

bool ProcessTable::add(const std::shared_ptr<Process>& process) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (byName_.count(process->name())) 
        return false;

    byName_[process->name()] = process;
    ordered_.push_back(process);
    
    return true;
}

std::shared_ptr<Process> ProcessTable::find(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = byName_.find(name);
    return it == byName_.end() ? nullptr : it->second;
}

bool ProcessTable::exists(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return byName_.count(name) > 0;
}

std::vector<std::shared_ptr<Process>> ProcessTable::all() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return ordered_;
}

size_t ProcessTable::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return ordered_.size();
}