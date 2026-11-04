#include "utils/Logger.h"
#include "utils/Timestamp.h"

std::string Logger::formatPrintLog(int coreId, const std::string& message) {
    return "(" + Timestamp::now() + ") Core:" + std::to_string(coreId) +
           " \"" + message + "\"";
}