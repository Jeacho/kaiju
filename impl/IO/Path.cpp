
#include "kaiju/IO/Path.h"

using namespace kaiju;

#if defined(_WIN32) || defined(_WIN64)
# define WINDOWS
#elif defined(__APPLE__)
# define MACOS
#elif defined(__unix__) || defined(__unix)
# define LINUX
#endif

std::ostream &kaiju::operator<<(std::ostream &os, const Path &path) {
    return os << path.str();
}

#undef WINDOWS
#undef MACOS
#undef LINUX
