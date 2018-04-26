
#include "kaiju/IO/MemoryBuffer.h"

#include <fstream>

using namespace kaiju;

// \brief Attempts to read in a file of the specified path, returns null
// if an error occured while reading.
std::optional<MemoryBuffer> MemoryBuffer::read(const Path &path) {
    if (!path.exists()) {
        std::cout << io::brRed << io::bold << "error: " << io::reset
            << "the path \'" << path.str() << "\' is not a file or directory."
            << std::endl;
        return std::nullopt;
    }

    if (path.is_dir()) {
        std::cout << io::brRed << io::bold << "error: " << io::reset
            << "the path \'" << path.str() << "\' is a directory."
            << std::endl;
        return std::nullopt;
    }

    std::ifstream stream(path.c_str(), std::ios::ate | std::ios::in | std::ios::binary);
    std::size_t fsize = stream.tellg();
    stream.seekg(0, stream.beg);

    if (!stream.is_open()) {
        return std::nullopt;
    }

    char *memory = static_cast<char *>(operator new(fsize + 1));

    if (!memory) {
		return std::nullopt;
    }

    memory[fsize] = '\0'; ///< increment size just incase.
    stream.read(memory, fsize);


    return MemoryBuffer(&memory[0], &memory[fsize + 1]);
}

// \brief Get's this specified offsets line number and column number.
std::pair<std::size_t, std::size_t>
MemoryBuffer::getPositionalData(const char *loc) const {
    assert(loc >= begin() && loc <= end()
        && "The memory address provided is not contained within this buffer.");

    std::size_t lineNo = 1;             //< The line number of the current read
                                        //< indicator.
    const char *newLnPtr = begin();     //< The memory location of the current
                                        //< start of the line.

    const char *indicator;
    for (indicator = begin(); indicator != loc; indicator++) {
        if (*indicator == '\n' || *indicator == '\r') {
            newLnPtr = indicator + 1;
            lineNo++;
        }
    }

    return std::pair<std::size_t, std::size_t>(lineNo,
        (indicator - newLnPtr));
}

std::pair<std::size_t, std::size_t>
MemoryBuffer::getPositionalData(SourceLoc loc) const {
    assert(loc.get() >= begin() && loc.get() <= end()
        && "The memory address provided is not contained within this buffer.");
    return getPositionalData(loc.get());
}

// \brief Given a memory location this method returns the source line that
// the provided memory location is on.
StringRef MemoryBuffer::getLineFromLoc(const char *loc) const {
    assert(loc >= begin() && loc <= end()
        && "The memory address provided is not contained within this buffer.");

    const char *lnBegin = loc,  //< The begining
               *lnEnd   = loc;  //< and end of the returned line.

    for (/* lnBegin */; lnBegin != begin(); lnBegin--)
        if (*lnBegin == '\n' || *lnBegin == '\r')
            break;

    for (/* lnEnd */; lnEnd != end(); lnEnd++)
        if (*lnEnd == '\n' || *lnEnd == '\r')
            break;

    return StringRef(lnBegin, (std::size_t)(lnEnd - lnBegin));
}

StringRef MemoryBuffer::getLineFromLoc(SourceLoc loc) const {
    assert(loc.get() >= begin() && loc.get() <= end()
        && "The memory address provided is not contained within this buffer.");
    return getLineFromLoc(loc.get());
}
