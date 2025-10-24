module;

#include <concepts>

export module ddge.modules.exec.locks.Lock;

import ddge.utility.TypeList;

namespace ddge::exec {

struct LockBase {};

export template <typename T>
struct Lock : LockBase {
    using LockedTypes = util::TypeList<T>;
};

export template <typename T>
concept lock_c = std::derived_from<T, LockBase>;

}   // namespace ddge::exec
