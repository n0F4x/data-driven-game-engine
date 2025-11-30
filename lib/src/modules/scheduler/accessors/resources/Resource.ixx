module;

#include <type_traits>

export module ddge.modules.scheduler.accessors.resources:Resource;

import ddge.modules.resources.resource_c;
import ddge.modules.scheduler.locks.CriticalSectionType;
import ddge.modules.scheduler.locks.Lock;
import ddge.modules.scheduler.locks.LockGroup;

import ddge.utility.containers.Ref;
import ddge.utility.meta.type_traits.const_like;

namespace ddge::scheduler::accessors {

inline namespace resources {

export template <typename Resource_T>
    requires ddge::resources::resource_c<std::remove_const_t<Resource_T>>
class Resource : public util::Ref<Resource_T> {
    using Base = util::Ref<Resource_T>;

public:
    using Underlying = Resource_T;

    constexpr static auto lock_group() -> const LockGroup&;

    using Base::Base;
};

}   // namespace resources

}   // namespace ddge::scheduler::accessors

template <typename Resource_T>
    requires ddge::resources::resource_c<std::remove_const_t<Resource_T>>
constexpr auto ddge::scheduler::accessors::resources::Resource<Resource_T>::lock_group()
    -> const LockGroup&
{
    static const LockGroup lock_group{ [] -> LockGroup {
        LockGroup          result;
        result.expand<Resource<std::remove_const_t<Resource_T>>>(
            Lock{
                std::is_const_v<Resource_T> ? CriticalSectionType::eShared
                                            : CriticalSectionType::eExclusive   //
            }   //
        );
        return result;
    }() };

    return lock_group;
}
