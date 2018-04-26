
#include "kaiju/Parse/Token.h"

using namespace kaiju;

namespace kaiju::tok {

} // namespace tok

std::ostream &kaiju::operator<<(std::ostream &os, const Token &tok) {
    return os << tok.getText().str();
}
