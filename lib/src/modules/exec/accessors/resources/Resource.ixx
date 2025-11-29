module;

#include <type_traits>

export module ddge.modules.exec.accessors.resources:Resource;

import ddge.modules.resources.resource_c;
import ddge.modules.exec.locks.CriticalSectionType;
import ddge.modules.exec.locks.Lock;
import ddge.modules.exec.locks.LockGroup;

import ddge.utility.containers.Ref;
import ddge.utility.meta.type_traits.const_like;

namespace ddge::exec::accessors {

inline namespace resources {

export template <typename Resource_T>
    requires ddge::resources::resource_c<std::remove_const_t<Resource_T>>
class Resource : public util::Ref<Resource_T> {
    using Base = util::Ref<Resource_T>;

public:
    using Underlying = Resource_T;

    constexpr static auto lock_group() -> LockGroup;

    using Base::Base;
};

}   // namespace resources

}   // namespace ddge::exec::accessors

template <typename Resource_T>
    requires ddge::resources::resource_c<std::remove_const_t<Resource_T>>
constexpr auto ddge::exec::accessors::resources::Resource<Resource_T>::lock_group()
    -> LockGroup
{
    constexpr Lock lock{
        std::is_const_v<Resource_T> ? CriticalSectionType::eShared
                                    : CriticalSectionType::eExclusive   //
    };

    LockGroup lock_group;
    lock_group.expand<Resource<std::remove_const_t<Resource_T>>>(auto{ lock });
    return lock_group;
}
