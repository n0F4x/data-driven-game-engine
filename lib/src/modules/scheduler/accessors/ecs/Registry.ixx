export module modules.scheduler.accessors.ecs.Registry;

import modules.ecs;

import utility.containers.Ref;

namespace modules::scheduler::accessors {

inline namespace ecs {

export class Registry : public util::Ref<modules::ecs::Registry> {
    using Base = util::Ref<modules::ecs::Registry>;

public:
    using Base::Base;
};

}   // namespace ecs

}   // namespace modules::scheduler::accessors
