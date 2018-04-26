
#ifndef KAIJU_SUPPORT_DIAGNOSTIC_H
#define KAIJU_SUPPORT_DIAGNOSTIC_H

#include <any>
#include <cassert>
#include <iostream>
#include <type_traits>

#include "kaiju/ADT/StringRef.h"

namespace kaiju {

namespace err {

// \brief Error Identifier enumerations
enum ErrorID {
#define DIAGNOSTIC(DIAGTYPE, DIAGID, TEXT) \
    DIAGID,
#include "kaiju/Support/DiagnosticCodes.def"
};

// \brief Get the Diagnostic Message mapped to the specified error code.
const char *message(ErrorID id);

} // namespace err

namespace detail {

// \brief RTTI for tracking the value stored inside the Argument aggregate.
enum ArgumentTypeInfo {
    SignedIntType,
    UnsignedIntType,
    SignedCharType,
    UnsignedCharType,
    StringRefType,
    VoidPointerType,
};

// \brief Union wrapper.
struct Argument {
    ArgumentTypeInfo type_info;

    union {
        int64_t     signed_integer;
        uint64_t    unsigned_integer;
        char        signed_char;
        unsigned char unsigned_char;
        StringRef   string_ref;
        void *      void_pointer;
    };

    Argument()
         : type_info(VoidPointerType),
           void_pointer(nullptr) { }

    Argument(Argument &other)
         : type_info(other.type_info),
           void_pointer(nullptr) {
    }

    Argument(char value)
         : type_info(SignedCharType),
           signed_char(value) { }

    Argument(StringRef value)
         : type_info(StringRefType),
           string_ref(value) { }
};

// \brief Template specialization for a cast to integral types.
template<typename T,
    typename = std::enable_if<std::is_integral<T>::value>>
T cast_argument(Argument &val);

// \brief Signed character specialization.
template<>
char cast_argument<char>(Argument &val);

// \brief Casts to strings.
template<>
std::string cast_argument<std::string>(Argument &val);

} // namespace detail

// Class Diagnostic
//
// \brief Diagnostic is runtime diagnostic interface. It stores metadata on a
// enumerated error message.
//
class Diagnostic final {
    friend class DiagnosticBuilder;

    // \brief The identifier mapped to static data that pertains to this Diagnostic.
    err::ErrorID ErrorCode;

    // \brief Arguments to be passed with this Diagnostic.
    detail::Argument *Arguments;
    std::size_t NoArgs; //< \brief The number of arguments stored in this class.

    // \brief Dispatch constructor for handling parameter packs.
	template <typename T, typename... Ts>
	void disptach(int i, T&& v, Ts&&... Args) {
		Arguments[i] = std::forward<T>(v);
		dispatch(++i, std::forward<Ts>(Args)...);
        NoArgs++;
    }
    void dispatch(int) { /* empty */ }

public:

    // ctor.
    template <typename... Ts>
    Diagnostic(err::ErrorID errorc, Ts&&... Args) : ErrorCode(errorc) {
        Arguments = new detail::Argument[10];
        disptach(0, std::forward<Ts>(Args)...);
    }

    Diagnostic(err::ErrorID errorc)
         : ErrorCode(errorc) { }

    // \brief Get the argument stored at the index specified.
    detail::Argument &getArgument(std::size_t i) const {
        assert(i >= 0 && i <= NoArgs
            && "bounds checking error. in getArgument");
        return Arguments[i];
    }

    // \brief Get the ID of this Diagnostic.
    err::ErrorID getErrorCode() const { return ErrorCode; }
};

} // namespace kaiju

#endif // KAIJU_SUPPORT_DIAGNOSTIC_H
