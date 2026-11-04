#pragma once
#include <ctime>
#include <string>

namespace Timestamp {

// Current local time as "MM/DD/YYYY HH:MM:SSAM", e.g. "08/06/2024 09:15:22AM".
// Thread-safe. Callers add the surrounding parentheses when printing.
std::string now();

// Same format for an arbitrary time_t (e.g. a process's creation time).
std::string format(std::time_t t);

} // namespace Timestamp