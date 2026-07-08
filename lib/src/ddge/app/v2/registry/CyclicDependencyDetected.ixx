export module ddge.app.v2.registry.CyclicDependencyDetected;

import ddge.util.contracts;

namespace ddge::app::v2 {

export class CyclicDependencyDetected
    : public util::PreconditionViolation   //
{
    using PreconditionViolation::PreconditionViolation;
};

}   // namespace ddge::app::v2
