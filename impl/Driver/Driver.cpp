
#include <iostream>

#include "kaiju/IR/IR.h"
#include "kaiju/Parse/Lexer.h"

using namespace kaiju;

int main(int argc, char const *argv[]) {
    if (argc < 2)
        exit(1);

    std::optional<MemoryBuffer> maybeBuffer = MemoryBuffer::read(argv[1]);
    Path path(argv[1]);

    if (!maybeBuffer)
        exit(1);

    TranslationUnit unit(path, *maybeBuffer);
    DiagnosticBuilder db(unit);

    Lexer lexer(unit, db);
    std::cout << lexer.scan() << std::endl;

    return 0;
}
