module;

#include <type_traits>

export module ddge.modules.exec.locks.WriterLock;

import ddge.modules.exec.locks.CompositeLock;
import ddge.modules.exec.locks.ReaderLock;

import ddge.utility.TypeList;

namespace ddge::exec {

export template <typename T>
    requires(!std::is_const_v<T>)
struct WriterLock : CompositeLock<WriterLock<T>, LockDependencyList<ReaderLock<T>>> {};

}   // namespace ddge::exec
