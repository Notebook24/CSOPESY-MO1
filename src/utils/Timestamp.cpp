#include "utils/Timestamp.h"

#include <chrono>
#include <ctime>

std::string Timestamp::now() {
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tmv{};
#ifdef _WIN32
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%m/%d/%Y %I:%M:%S%p", &tmv);
    return std::string(buf);
}