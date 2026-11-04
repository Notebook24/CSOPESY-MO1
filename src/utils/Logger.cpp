#include "utils/Logger.h"
#include "utils/Timestamp.h"

void Logger::log(const std::string& message, int coreId) {
    std::string entry = "(" + Timestamp::now() + ") Core:" + std::to_string(coreId) +
                        " \"" + message + "\"";
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.push_back(std::move(entry));
}

std::vector<std::string> Logger::getEntries() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return entries_;
}

std::size_t Logger::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return entries_.size();
}

void Logger::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.clear();
}