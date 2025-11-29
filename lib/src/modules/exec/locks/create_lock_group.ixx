export module ddge.modules.exec.locks.create_lock_group;

import ddge.modules.exec.locks.has_lock_group_c;
import ddge.modules.exec.locks.LockGroup;

import ddge.utility.meta.concepts.type_list.type_list_all_of;
import ddge.utility.meta.algorithms.for_each;

namespace ddge::exec {

template <typename T>
struct HasLockGroup {
    constexpr static bool value = has_lock_group_c<T>;
};

export template <util::meta::type_list_all_of_c<HasLockGroup> TypeList_T>
[[nodiscard]]
constexpr auto create_lock_group() -> LockGroup
{
    LockGroup result;
    util::meta::for_each<TypeList_T>([&result]<typename T> -> void {
        result.expand(T::lock_group());
    });
    return result;
}

}   // namespace ddge::exec
