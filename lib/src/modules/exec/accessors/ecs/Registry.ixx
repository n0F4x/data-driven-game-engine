export module ddge.modules.exec.accessors.ecs.Registry;

import ddge.modules.ecs;
import ddge.modules.exec.locks.CriticalSectionType;
import ddge.modules.exec.locks.Lock;
import ddge.modules.exec.locks.LockGroup;

import ddge.utility.containers.Ref;

namespace ddge::exec::accessors {

inline namespace ecs {

export class Registry : public util::Ref<ddge::ecs::Registry> {
    using Base = util::Ref<ddge::ecs::Registry>;

public:
    constexpr static auto lock_group() -> LockGroup;

    using Base::Base;
};

}   // namespace ecs

}   // namespace ddge::exec::accessors

constexpr auto ddge::exec::accessors::ecs::Registry::lock_group() -> LockGroup
{
    LockGroup lock_group;
    lock_group.expand<Registry>(Lock{ CriticalSectionType::eExclusive });
    return lock_group;
}
