export module ddge.modules.exec.accessors.assets:Cached;

import ddge.modules.assets;
import ddge.modules.exec.locks.CriticalSectionType;
import ddge.modules.exec.locks.Lock;
import ddge.modules.exec.locks.LockGroup;

import ddge.utility.containers.Ref;

namespace ddge::exec::accessors {

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

}   // namespace ddge::exec::accessors

template <ddge::assets::loader_c Loader_T>
constexpr auto ddge::exec::accessors::assets::Cached<Loader_T>::lock_group() -> LockGroup
{
    LockGroup lock_group;
    lock_group.expand<Cached>(Lock{ CriticalSectionType::eExclusive });
    return lock_group;
}
