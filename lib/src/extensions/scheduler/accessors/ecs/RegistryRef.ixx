export module extensions.scheduler.accessors.ecs.RegistryRef;

import core.ecs;

import utility.containers.Ref;

namespace extensions::scheduler::accessors::ecs {

export class RegistryRef : public util::Ref<core::ecs::Registry> {
    using Base = util::Ref<core::ecs::Registry>;

public:
    using Base::Base;
};

}   // namespace extensions::scheduler::accessors::ecs
