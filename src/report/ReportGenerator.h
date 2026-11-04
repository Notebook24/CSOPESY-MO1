#pragma once
#include <string>
#include "process/ProcessTable.h"
#include "scheduler/Scheduler.h"

// Owner: Ryan
// Used by "screen -ls" (print the text) and "report-util" (save the same text).
// Reads only: table.getAll() filtered by getState(), plus the scheduler's
// getTotalCores / getBusyCores / getAvailableCores / getUtilizationPercent.
class ReportGenerator {
public:
    static std::string buildReport(const ProcessTable& table, const Scheduler& scheduler);

    // Writes text to path (e.g. "csopesy-log.txt"). Returns false on failure.
    static bool saveToFile(const std::string& path, const std::string& text);
};
