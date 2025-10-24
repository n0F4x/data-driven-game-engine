export module ddge.modules.exec.accessors.ecs:locks.ExclusiveComponentLock;

import :locks.SharedComponentLock;

import ddge.modules.ecs;
import ddge.modules.exec.locks.CompositeLock;

namespace ddge::exec::accessors {

inline namespace ecs {

export template <ddge::ecs::queryable_component_c Component_T>
struct ExclusiveComponentLock
    : CompositeLock<
          ExclusiveComponentLock<Component_T>,
          LockDependencyList<SharedComponentLock<Component_T>>> {};

}   // namespace ecs

}   // namespace ddge::exec::accessors
