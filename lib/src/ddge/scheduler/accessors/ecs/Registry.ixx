export module ddge.scheduler.accessors.ecs.Registry;

import ddge.ecs;
import ddge.scheduler.locks.CriticalSectionType;
import ddge.scheduler.locks.Lock;
import ddge.scheduler.locks.LockGroup;

import ddge.util.containers.Ref;

namespace ddge::scheduler::accessors {

inline namespace ecs {

export class Registry : public util::Ref<ddge::ecs::Registry> {
    using Base = util::Ref<ddge::ecs::Registry>;

public:
    constexpr static auto lock_group() -> const LockGroup&;

    using Base::Base;
};

}   // namespace ecs

}   // namespace ddge::scheduler::accessors

constexpr auto ddge::scheduler::accessors::ecs::Registry::lock_group() -> const LockGroup&
{
    static const LockGroup lock_group{ [] -> LockGroup {
        LockGroup          result;
        result.expand<Registry>(Lock{ CriticalSectionType::eExclusive });
        return result;
    }() };

    return lock_group;
}
