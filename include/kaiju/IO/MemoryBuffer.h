
#ifndef KAIJU_IO_MEMORYBUFFER_H
#define KAIJU_IO_MEMORYBUFFER_H

#include <cassert>
#include <optional>

#include "kaiju/IO/Console.h"
#include "kaiju/IO/SourceLoc.h"
#include "kaiju/IO/Path.h"
#include "kaiju/ADT/StringRef.h"

namespace kaiju {

// Class MemoryBuffer
//
// \brief This class is meant to contain a block of allocated memory.
class MemoryBuffer {
    const char *m_begin,    //< Pointer to the beginning of the buffer.
               *m_end;      //< Pointer to the end of the buffer.

public:
    // Ctor
    MemoryBuffer(const char *be, const char *en)
         : m_begin(be), m_end(en) { /* empty */}

    const char *begin() const { return m_begin; }   //< Get beginning of buffer.
    const char *end()   const { return m_end;   }   //< Get end of buffer.

    /// \brief Returns the length of this buffer.
    std::size_t length() const { return (std::size_t)(m_end - m_begin); }

    // \brief For use with std::optional<T>.
    inline bool operator==(std::nullptr_t) const { return false; }

    // File I/O

    // \brief Attempts to read in a file of the specified path, returns null
    // if an error occured while reading.
    static std::optional<MemoryBuffer> read(const Path &path);

    // \brief Get's this specified offsets line number and column number.
    std::pair<std::size_t, std::size_t>
    getPositionalData(const char *loc) const;

    std::pair<std::size_t, std::size_t>
    getPositionalData(SourceLoc loc) const;

    // \brief Given a memory location this method returns the source line that
    // the provided memory location is on.
    StringRef getLineFromLoc(const char *loc) const;

    StringRef getLineFromLoc(SourceLoc loc) const;
};

} // namespace kaiju

#endif // KAIJU_IO_MEMORYBUFFER_H
