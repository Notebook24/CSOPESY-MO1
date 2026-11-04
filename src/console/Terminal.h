#pragma once
#include <cstdlib>

namespace Terminal {
inline void clear() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}
}  // namespace Terminal
