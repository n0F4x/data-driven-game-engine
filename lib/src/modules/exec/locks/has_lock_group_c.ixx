module;

#include <concepts>
#include <type_traits>

export module ddge.modules.exec.locks.has_lock_group_c;

import ddge.modules.exec.locks.LockGroup;

namespace ddge::exec {

export template <typename T>
concept has_lock_group_c = requires {
    std::same_as<std::remove_cvref_t<decltype(T::lock_group())>, LockGroup>;
};

}   // namespace ddge::exec
