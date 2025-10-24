export module ddge.modules.exec.accessors.ecs:locks.ExclusiveRegistryLock;

import :locks.SharedRegistryLock;

import ddge.modules.exec.locks.CompositeLock;

namespace ddge::exec::accessors {

inline namespace ecs {

export struct ExclusiveRegistryLock
    : CompositeLock<ExclusiveRegistryLock, LockDependencyList<SharedRegistryLock>> {};

}   // namespace ecs

}   // namespace ddge::exec::accessors
