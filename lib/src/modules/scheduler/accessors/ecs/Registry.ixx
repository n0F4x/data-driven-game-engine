export module ddge.modules.scheduler.accessors.ecs.Registry;

import ddge.modules.ecs;

import ddge.utility.containers.Ref;

namespace ddge::scheduler::accessors {

inline namespace ecs {

export class Registry : public util::Ref<ddge::ecs::Registry> {
    using Base = util::Ref<ddge::ecs::Registry>;

public:
    using Base::Base;
};

}   // namespace ecs

}   // namespace ddge::scheduler::accessors
