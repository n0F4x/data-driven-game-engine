module;

#include <concepts>

export module ddge.modules.exec.locks.CompositeLock;

import ddge.modules.exec.locks.Lock;

import ddge.utility.meta.concepts.type_list.type_list_contains;
import ddge.utility.meta.type_traits.type_list.type_list_join;
import ddge.utility.meta.type_traits.type_list.type_list_push_front;
import ddge.utility.meta.type_traits.type_list.type_list_size;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.meta.type_traits.type_list.type_list_unique;
import ddge.utility.TypeList;

namespace ddge::exec {

struct LockDependenciesBase {};

export template <lock_c... Locks_T>
struct LockDependencyList : LockDependenciesBase {
    using type = util::TypeList<Locks_T...>;
};

template <typename T>
struct GetLockedTypes {
    using type = T::LockedTypes;
};

struct CompositeLockBase {};

export template <typename T, std::derived_from<LockDependenciesBase> Dependencies_T>
    requires(!util::meta::type_list_contains_c<typename Dependencies_T::type, T>)
             && (util::meta::type_list_size_v<typename Dependencies_T::type> > 0)
struct CompositeLock : Lock<T>, CompositeLockBase {
    using LockedTypes = util::meta::type_list_unique_t<util::meta::type_list_push_front_t<
        util::meta::type_list_join_t<
            util::meta::type_list_transform_t<typename Dependencies_T::type, GetLockedTypes>>,
        T>>;
};

export template <typename T>
concept composite_lock_c = lock_c<T> && std::derived_from<T, CompositeLockBase>;

}   // namespace ddge::exec
