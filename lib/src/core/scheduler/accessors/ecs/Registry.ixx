export module core.scheduler.accessors.ecs.Registry;

import core.ecs;

import utility.containers.Ref;

namespace core::scheduler::accessors {

inline namespace ecs {

export class Registry : public util::Ref<core::ecs::Registry> {
    using Base = util::Ref<core::ecs::Registry>;

public:
    using Base::Base;
};

}   // namespace ecs

}   // namespace core::scheduler::accessors
