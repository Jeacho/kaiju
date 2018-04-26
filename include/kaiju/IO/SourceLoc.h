
#ifndef KAIJU_IO_SOURCELOC_H
#define KAIJU_IO_SOURCELOC_H

namespace kaiju {

// \brief This is a constant char * wrapper that is used to represent locations
// within some arbitrary files buffer.
class SourceLoc {
    const char *Ptr = nullptr;

public:
    // Ctor
    SourceLoc(const char *P)
        : Ptr(P) { /* empty */ }

    // Copy
    SourceLoc(SourceLoc &SL)
        : Ptr(SL.get()) { /* empty */ }

    bool isValid() const { return Ptr != nullptr; }
    const char *get() const { return Ptr; }

    // \brief This method creates a new pointer from this one plus some offset.
    SourceLoc getFromOffset(std::size_t Offset) const {
        assert(isValid());
        return SourceLoc(Ptr + Offset);
    }

    // Operator overloads
    bool operator==(const SourceLoc &RHS) const { return RHS.Ptr == Ptr; }
    bool operator!=(const SourceLoc &RHS) const { return RHS.Ptr != Ptr; }
};

// \brief This represents a range between two pointers within a buffer.
namespace detail {

class SourceRangeBase {
protected:
    const SourceLoc Begin,    //< The beginning of thie range.
                    End;      //< The end of this range.

    // \brief Inline check to make sure that Loc isnt null, this is used within
    // the constructor to keep nullptr SourceLocs from being used within ranges.
    inline SourceLoc validate(SourceLoc Loc) const {
        assert(Loc.isValid() && "Attempt to form range from null loc");
        return Loc;
    }

protected:
    SourceRangeBase(SourceLoc Be, SourceLoc En)
        : Begin(validate(Be)), End(validate(En)) { /* empty */ }

    // Getters
    const SourceLoc &begin()   const { return Begin; }
    const SourceLoc &end()     const { return End; }

    // Util
    std::size_t length() const { return End.get() - Begin.get(); }
};

} // namespace detail

class SourceRange : public detail::SourceRangeBase {
public:
    // Ctors
    SourceRange(SourceLoc Be, SourceLoc En)
        : SourceRangeBase(Be, En) { /* empty */ }

    SourceRange(SourceLoc Be, std::size_t Len)
        : SourceRangeBase(Be, Be.getFromOffset(Len)) { /* empty */ }

    SourceRange(const char *Be, const char *En)
        : SourceRangeBase(SourceLoc(Be), SourceLoc(En)) { /* empty */ }

    SourceRange(const char *Be, std::size_t Len)
        : SourceRangeBase(SourceLoc(Be), SourceLoc(Be + Len)) { /* empty */ }
};

} // namespace kaiju

#endif // KAIJU_IO_SOURCELOC_H
