
#ifndef KAIJU_IO_PATH_H
#define KAIJU_IO_PATH_H

#include <cassert>
#include <cstring>
#include <type_traits>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
# define WINDOWS
# ifndef S_ISREG
#  define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
# endif
# ifndef S_ISDIR
#  define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
# endif
#elif defined(__APPLE__)
# define MACOS
#elif defined(__unix__) || defined(__unix)
# define LINUX
#endif

#if defined(MACOS) || defined(LINUX)
# include <sys/stat.h>
#elif defined(WINDOWS)
# include <io.h>
# include <windows.h>
#endif

namespace kaiju {

// Class Path
//
// \brief This class implements a immutable path.
class Path {

    /// The path this path object pertains to.
    const char *m_str;

    // The length of this path in characters.
    const std::size_t m_length;

    // \brief This is inline check to make sure we dont build null paths.
    inline const char *check(const char *cstr) const {
        assert(cstr != nullptr && "cannot construct path from nullptr.");
        return cstr;
    }

public:

    Path(std::string string)
        : m_str(string.c_str()), m_length(string.length()) { /* empty */ }

    Path(const char *cstr)
        : m_str(check(cstr)), m_length(strlen(cstr)) { /* empty */ }

    ////////////////////////////////////////////////////////////////////////////
    ///            Static defintions for checking file attributes.           ///
    ////////////////////////////////////////////////////////////////////////////

    // \brief checks if this path is a file.
    static inline bool is_file(const char *p) {
#if defined(MACOS) || defined(LINUX)
        struct stat s;
        if(stat(p, &s) != 0)
            return false;
        return S_ISREG(s.st_mode);
#elif defined(WINDOWS)
        DWORD fileAttributes = GetFileAttributesA(p);

        // Path does not exist
        if (fileAttributes == INVALID_FILE_ATTRIBUTES)
            return false;

        if (fileAttributes == FILE_ATTRIBUTE_FILE)
            return true;

        return false;
#endif
    }

    // \brief checks if this path is a directory.
    static inline bool is_dir(const char *p) {
#if defined(MACOS) || defined(LINUX)
        struct stat s;
        if(stat(p, &s) != 0)
            return false;
        return S_ISDIR(s.st_mode);
#elif defined(WINDOWS)
        DWORD fileAttributes = GetFileAttributesA(p);

        // Path does not exist
        if (fileAttributes == INVALID_FILE_ATTRIBUTES)
            return false;

        if (fileAttributes == FILE_ATTRIBUTE_DIRECTORY)
            return true;

        return false;
#endif
    }

    // \brief checks if this path exists.
    static inline bool exists(const char *p) {
        struct stat s;
        return (stat(p, &s) == 0);
    }

    // \brief Returns whether the string specified has valid syntax for a path.
    static bool isValidPath();

    ////////////////////////////////////////////////////////////////////////////
    ///                   Non-static attribute check wrappers                ///
    ////////////////////////////////////////////////////////////////////////////

    inline bool exists() const  { return exists(m_str);   }
    inline bool is_dir() const  { return is_dir(m_str);   }
    inline bool is_file() const { return is_file(m_str);  }

    ////////////////////////////////////////////////////////////////////////////
    ///                             String casts                             ///
    ////////////////////////////////////////////////////////////////////////////

    const char *c_str()     const { return m_str;               } //< c-str
    std::string str()       const { return std::string(m_str);  }
    operator std::string()  const { return str();               }

    template <typename T>
    typename std::enable_if<std::is_same<T, std::string>::value,
                            Path>::type &
    operator=(T &&Str) = delete;

    std::ostream &operator<<(std::ostream &os) {
        return os << str();
    }
};

std::ostream &operator<<(std::ostream &os, const Path &path);

} // namespace kaiju

#endif // KAIJU_IO_PATH_H
