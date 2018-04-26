
#ifndef KAIJU_IO_CONSOLE_H
#define KAIJU_IO_CONSOLE_H

#if defined(_WIN32) || defined(_WIN64)
# define WINDOWS
#elif defined(__APPLE__)
# define MACOS
#elif defined(__unix__) || defined(__unix)
# define LINUX
#endif

#if defined(MACOS) || defined(LINUX)
# include <unistd.h>
#elif defined(WINDOWS)
# include <io.h>
# include <windows.h>
#endif

#include <iostream>
#include <cstdio>

namespace kaiju {

// \brief This namespace provides several utility functions for text-based
// manipulation within terminals.
namespace io {

// An index to be used to access a private storage of I/O streams. See
// colorize / nocolorize I/O manipulators for details.
static int colorize_index = std::ios_base::xalloc();

inline bool hasAttribute(const std::ostream &stream);

// \brief Returns a file pointer to the file if \p stream is a standard stream,
// otherwise it returns a nullptr.
inline FILE* getStandardStream(const std::ostream &stream) {
    if (&stream == &std::cout)
        return stdout;
    else if ((&stream == &std::cerr) || (&stream == &std::clog))
        return stderr;

    return nullptr;
}

// \brief Sets whether this stream can be colored or not.
inline std::ostream &colorize(std::ostream &stream, bool state) {
    stream.iword(colorize_index) = (long)(state);
    return stream;
}

// \brief Returns whether the stream specified can be colored.
inline bool isColorized(std::ostream &stream) {
    return hasAttribute(stream)
        || static_cast<bool>(stream.iword(colorize_index));
}

// \brief Returns if the stream specified is a standard stream.
inline bool hasAttribute(const std::ostream &stream) {
    FILE* std_stream = getStandardStream(stream);

    if (!std_stream)
        return false;

#if defined(MACOS) || defined(LINUX)
    return ::isatty(fileno(std_stream));
#elif defined(WINDOWS)
    return ::_isatty(_fileno(std_stream));
#endif
}

#if defined(WINDOWS)

// \brief Sets the windows console's color attributes.
inline void setWinAttributes(std::ostream &stream,
                                  int foreground,
                                  int background) {
    static WORD defaultAttributes = 0;

    if (!hasAttribute(stream))
        return;

    HANDLE hTerminal = INVALID_HANDLE_VALUE;
    if (&stream == &std::cout)
        hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    else if (&stream == &std::cerr)
        hTerminal = GetStdHandle(STD_ERROR_HANDLE);

    if (!defaultAttributes) {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(hTerminal, &info))
            return;
        defaultAttributes = info.wAttributes;
    }

    if (foreground == -1 && background == -1) {
        SetConsoleTextAttribute(hTerminal, defaultAttributes);
        return;
    }

    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(hTerminal, &info))
        return;

    if (foreground != -1) {
        info.wAttributes &= ~(info.wAttributes & 0x0F);
        info.wAttributes |= static_cast<WORD>(foreground);
    }

    if (background != -1) {
        info.wAttributes &= ~(info.wAttributes & 0xF0);
        info.wAttributes |= static_cast<WORD>(background);
    }

    SetConsoleTextAttribute(hTerminal, info.wAttributes);
}
#endif // WINDOWS

// \brief Reset the terminal formatting.
inline std::ostream &reset(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[00m";
#elif defined(WINDOWS)
        setWinAttributes(stream, -1, -1);
#endif
    }

    return stream;
}

inline std::ostream &bold(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[1m";
#elif defined(WINDOWS)
    // Not supported
#endif
    }

    return stream;
}

inline std::ostream &dark(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[2m";
#elif defined(WINDOWS)
    // Not supported
#endif
    }

    return stream;
}

inline std::ostream &underline(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[4m";
#elif defined(WINDOWS)
    // Not supported
#endif
    }

    return stream;
}


inline std::ostream &blink(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[5m";
#elif defined(WINDOWS)
    // Not supported
#endif
    }

    return stream;
}


inline std::ostream &reverse(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[7m";
#elif defined(WINDOWS)
    // Not supported
#endif
    }

    return stream;
}


inline std::ostream &concealed(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[8m";
#elif defined(WINDOWS)
    // Not supported
#endif
    }

    return stream;
}


inline std::ostream &grey(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[30m";
#elif defined(WINDOWS)
        winSetAttribute(stream, 0   /* grey */);
#endif
    }

    return stream;
}

inline std::ostream &brGrey(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[90m";
#elif defined(WINDOWS)
        winSetAttribute(stream, 0   /* grey */);
#endif
    }

    return stream;
}

inline std::ostream &red(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[31m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_RED);
#endif
    }

    return stream;
}

inline std::ostream &brRed(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[91m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_RED);
#endif
    }

    return stream;
}

inline std::ostream &green(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[32m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_GREEN);
#endif
    }

    return stream;
}

inline std::ostream &brGreen(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[92m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_GREEN);
#endif
    }

    return stream;
}

inline std::ostream &yellow(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[33m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_GREEN | FG_RED);
#endif
    }

    return stream;
}

inline std::ostream &brYellow(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[63m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_GREEN | FG_RED);
#endif
    }

    return stream;
}

inline std::ostream &blue(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[34m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_BLUE);
#endif
    }

    return stream;
}

inline std::ostream &brBlue(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[64m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_BLUE);
#endif
    }

    return stream;
}

inline std::ostream &magenta(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[35m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_BLUE | FG_RED);
#endif
    }

    return stream;
}

inline std::ostream &brMagenta(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[95m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_BLUE | FG_RED);
#endif
    }

    return stream;
}

inline std::ostream &cyan(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[36m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_BLUE | FG_GREEN);
#endif
    }

    return stream;
}

inline std::ostream &brCyan(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[96m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_BLUE | FG_GREEN);
#endif
    }

    return stream;
}

inline std::ostream &white(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[37m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_BLUE | FG_GREEN | FG_RED);
#endif
    }

    return stream;
}

inline std::ostream &brWhite(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[97m";
#elif defined(WINDOWS)
        winSetAttribute(stream, FG_BLUE | FG_GREEN | FG_RED);
#endif
    }

    return stream;
}

inline std::ostream &bgGrey(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[40m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, 0 /* grey */);
#endif
    }

    return stream;
}

inline std::ostream &brgGrey(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[101m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, 0 /* grey */);
#endif
    }

    return stream;
}

inline std::ostream &bgRed(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[41m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_RED);
#endif
    }

    return stream;
}

inline std::ostream &brgRed(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[101m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_RED);
#endif
    }

    return stream;
}

inline std::ostream &bgGreen(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[42m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_GREEN);
#endif
    }

    return stream;
}

inline std::ostream &brgGreen(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[102m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_GREEN);
#endif
    }

    return stream;
}

inline std::ostream &bgYellow(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[43m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_GREEN | BG_RED);
#endif
    }

    return stream;
}

inline std::ostream &brgYellow(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[103m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_GREEN | BG_RED);
#endif
    }

    return stream;
}

inline std::ostream &bgBlue(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[44m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_BLUE);
#endif
    }

    return stream;
}

inline std::ostream &brgBlue(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[104m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_BLUE);
#endif
    }

    return stream;
}

inline std::ostream &bgMagenta(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[45m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_BLUE | BG_RED);
#endif
    }

    return stream;
}

inline std::ostream &brgMagenta(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[105m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_BLUE | BG_RED);
#endif
    }

    return stream;
}

inline std::ostream &bgCyan(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[46m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_GREEN | BG_BLUE);
#endif
    }

    return stream;
}

inline std::ostream &brgCyan(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[106m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_GREEN | BG_BLUE);
#endif
    }

    return stream;
}

inline std::ostream &bgWhite(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[47m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_GREEN | BG_BLUE | BG_RED );
#endif
    }

    return stream;
}

inline std::ostream &brgWhite(std::ostream &stream) {
    if (isColorized(stream)) {
#if defined(MACOS) || defined(LINUX)
        stream << "\033[107m";
#elif defined(WINDOWS)
        winSetAttribute(stream, -1, BG_GREEN | BG_BLUE | BG_RED );
#endif
    }

    return stream;
}

} // namespace io

} // namespace kaiju

#undef WINDOWS
#undef MACOS
#undef LINUX

#endif // KAIJU_IO_CONSOLE_H
