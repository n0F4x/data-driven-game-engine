export module extensions.scheduler.accessors.ecs.Registry;

import core.ecs;

import utility.containers.Ref;

namespace extensions::scheduler::accessors::ecs {

export class Registry : public util::Ref<core::ecs::Registry> {
    using Base = util::Ref<core::ecs::Registry>;

public:
    using Base::Base;
};

}   // namespace extensions::scheduler::accessors::ecs
