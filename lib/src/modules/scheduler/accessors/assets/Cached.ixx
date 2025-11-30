export module ddge.modules.scheduler.accessors.assets:Cached;

import ddge.modules.assets;
import ddge.modules.scheduler.locks.CriticalSectionType;
import ddge.modules.scheduler.locks.Lock;
import ddge.modules.scheduler.locks.LockGroup;

import ddge.utility.containers.Ref;

namespace ddge::scheduler::accessors {

inline namespace assets {

export template <ddge::assets::loader_c Loader_T>
class Cached : public util::Ref<ddge::assets::Cached<Loader_T>> {
    using Base = util::Ref<ddge::assets::Cached<Loader_T>>;

public:
    using Loader = Loader_T;

    constexpr static auto lock_group() -> LockGroup;

    using Base::Base;
};

}   // namespace assets

}   // namespace ddge::scheduler::accessors

template <ddge::assets::loader_c Loader_T>
constexpr auto ddge::scheduler::accessors::assets::Cached<Loader_T>::lock_group()
    -> LockGroup
{
    LockGroup lock_group;
    lock_group.expand<Cached>(Lock{ CriticalSectionType::eExclusive });
    return lock_group;
}
