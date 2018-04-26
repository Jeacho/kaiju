
#include "kaiju/IR/Value.h"

using namespace kaiju;

#include "kaiju/IR/Context.h"

/// \brief Return a constant reference to the value's name.
///
/// This guaranteed to return the same reference as long as the value is not
/// modified.  If the value has a name, this does a hashtable lookup, so
/// it's not free.
StringRef Value::getName() const {
    if (!hasName)
        return StringRef("", 0);

    Context &Ctx = ValueType->getContext();
    auto query = Ctx.ValueNames.find(this);
    assert (query != Ctx.ValueNames.end()
        && "No name bound, there should be...");

    return query->second;
}

/// \brief Change the name of the value.
///
/// Choose a new unique name if the provided name is taken.
///
/// \param Name The new name; or "" if the value's name should be removed.
void Value::setName(StringRef name) {
    assert(!hasNameBinding() && "hasName out of sync.");
    Context &Ctx = ValueType->getContext();
    auto query = Ctx.ValueNames[this] = name;
    hasName = true;
}
