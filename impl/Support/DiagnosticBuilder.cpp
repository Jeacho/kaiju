
#include "kaiju/Support/DiagnosticBuilder.h"

using namespace kaiju;

#include <cassert>
#include <iostream>
#include <iomanip>

#include "kaiju/IO/Console.h"

// \brief Flush the current diagnostic in-flight.
void DiagnosticBuilder::flush() {
    assert(hasDiag && "no diagnostic in-flight.");
    std::string report = err::message(Diag->getErrorCode());

    for (int i = report.find('%');
             i != std::string::npos;
             i = report.find('%', i + 1)) {
        assert (isdigit(report[i + 1]) && "argument index is not a number.");
        char argid      = report[i + 1] - '0';
        detail::Argument &arg = Diag->getArgument(argid);

        report.replace(i, 2, detail::cast_argument<std::string>(arg));
    }

    std::cerr << io::bold << Unit.getPath() << ": " << io::reset
        << io::bold << io::brRed << "ERR" << std::setw(4) << std::setfill('0')
        << static_cast<int>(Diag->getErrorCode())  << ": " << io::reset
        << report
        << std::endl;
}
