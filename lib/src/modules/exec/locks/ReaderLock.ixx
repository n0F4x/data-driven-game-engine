module;

#include <type_traits>

export module ddge.modules.exec.locks.ReaderLock;

import ddge.modules.exec.locks.Lock;

namespace ddge::exec {

export template <typename T>
    requires(!std::is_const_v<T>)
struct ReaderLock : Lock<ReaderLock<T>> {};

}   // namespace ddge::exec
