export module ddge.registry.CyclicDependencyDetected;

import ddge.util.contracts;

namespace ddge::registry {

export class CyclicDependencyDetected
    : public util::PreconditionViolation   //
{
    using PreconditionViolation::PreconditionViolation;
};

}   // namespace ddge::registry
