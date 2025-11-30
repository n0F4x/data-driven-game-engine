module;

#include <concepts>
#include <type_traits>

export module ddge.modules.scheduler.locks.has_lock_group_c;

import ddge.modules.scheduler.locks.LockGroup;

namespace ddge::scheduler {

export template <typename T>
concept has_lock_group_c = requires {
    std::same_as<std::remove_cvref_t<decltype(T::lock_group())>, LockGroup>;
};

}   // namespace ddge::scheduler
