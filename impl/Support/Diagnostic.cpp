
#include "kaiju/Support/Diagnostic.h"

using namespace kaiju;

namespace kaiju {

namespace detail {

// \brief Template specialization for a cast to integral types.
template<typename T,
    typename = std::enable_if<std::is_integral<T>::value>>
T cast_argument(Argument &val) {
    if (val.type_info == SignedIntType)
        throw "bad cast_argument";
    return val.signed_integer;
};

// \brief StringRef specialization
template<>
StringRef cast_argument<StringRef>(Argument &val) {
    if (val.type_info == StringRefType)
        throw "bad cast_argument";
    return val.string_ref;
}

// \brief Signed character specialization.
template<>
char cast_argument<char>(Argument &val) {
    if (val.type_info == SignedCharType)
        throw "bad cast_argument";
    return val.signed_char;
}

// \brief Casts to strings.
template<>
std::string cast_argument<std::string>(Argument &val) {
    switch (val.type_info) {
    case SignedIntType:
        return std::to_string(val.signed_integer);

    case UnsignedIntType:
        return std::to_string(val.unsigned_integer);

    case SignedCharType:
        return std::string(1, val.signed_char);

    case StringRefType:
        return val.string_ref.str();

    case VoidPointerType:
        return "nullptr";
    }
}

}

namespace err {

// Diagnostic messages
static const char *messages[] = {
#define DIAGNOSTIC(DIAGTYPE, DIAGID, TEXT) \
    TEXT,
#include "kaiju/Support/DiagnosticCodes.def"
};

// \brief Get the Diagnostic Message mapped to the specified error code.
const char *message(ErrorID id) {
    return messages[static_cast<int>(id)];
}

} // namespace err

} // namespace kaiju
