#pragma once
#include <string>

class ProcessTable;
class Scheduler;

// Builds the text shown by "screen -ls" and saved by "report-util".
class ReportGenerator {
public:
    static std::string buildReport(const ProcessTable& table, const Scheduler& scheduler);

    // Overwrites the file at `path`. Returns false if it could not be written.
    static bool saveToFile(const std::string& path, const std::string& text);
};
