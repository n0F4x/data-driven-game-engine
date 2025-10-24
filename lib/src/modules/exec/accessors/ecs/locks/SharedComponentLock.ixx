export module ddge.modules.exec.accessors.ecs:locks.SharedComponentLock;

import :locks.SharedRegistryLock;

import ddge.modules.ecs;
import ddge.modules.exec.locks.Lock;

namespace ddge::exec::accessors {

inline namespace ecs {

export template <ddge::ecs::queryable_component_c Component_T>
struct SharedComponentLock : Lock<SharedComponentLock<Component_T>> {};

}   // namespace ecs

}   // namespace ddge::exec::accessors
