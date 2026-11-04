#include "utils/Timestamp.h"

#include <chrono>

namespace Timestamp {

std::string format(std::time_t t) {
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

std::string now() {
    return format(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
}

} // namespace Timestamp