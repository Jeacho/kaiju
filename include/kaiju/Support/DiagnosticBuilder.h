
#ifndef KAIJU_SUPPORT_DIAGNOSTICBUILD_H
#define KAIJU_SUPPORT_DIAGNOSTICBUILD_H

#include "kaiju/IR/TranslationUnit.h"
#include "kaiju/Support/Diagnostic.h"

#include <cassert>

namespace kaiju {

// Class DiagnosticBuilder
//
// \brief Class used for constructing and configuring a Diagnostic class.
//
class DiagnosticBuilder {

    // \brief The TranslationUnit context this class is emitting errors from.
    TranslationUnit &Unit;

    // \brief The current Diagnostic that is in-flight.
    Diagnostic *Diag;

    // \brief Whether a diagnostic is in-flight or not.
    bool hasDiag;

public:

    // ctor.
    DiagnosticBuilder(TranslationUnit &TU)
         : Unit(TU) {
        Diag = nullptr;
        hasDiag = false;
    }

    // \brief Creates a new Diagnostic and puts it in-flight.
    template <typename... Ts>
    Diagnostic *create(err::ErrorID errorc, Ts&&... Args) {
        Diagnostic *diag = new Diagnostic(errorc, Args...);
        hasDiag = true;
        Diag = diag;
        return diag;
    }

    // \brief Get's the current diagnostic if there is one in-flight.
    Diagnostic *getDiagnostic() const {
        assert(hasDiag && "No Diagnostic in-flight to retrieve.");
        return Diag;
    }

    // \brief Flush the current diagnostic in-flight.
    void flush();
};

}

#endif // KAIJU_SUPPORT_DIAGNOSTICBUILD_H
