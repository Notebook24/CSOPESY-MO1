#pragma once
#include <string>

// Owner: Alain
class Logger {
public:
    // Builds one PRINT log line:
    //   (10/06/2025 02:15:30PM) Core:0 "Hello world from p01!"
    // Process::addLog() calls this so every log looks the same.
    static std::string formatPrintLog(int coreId, const std::string& message);
};
