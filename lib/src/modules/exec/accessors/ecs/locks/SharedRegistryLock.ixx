export module ddge.modules.exec.accessors.ecs:locks.SharedRegistryLock;

import ddge.modules.ecs;
import ddge.modules.exec.locks.Lock;

namespace ddge::exec::accessors {

inline namespace ecs {

export struct SharedRegistryLock : Lock<SharedRegistryLock> {};

}   // namespace ecs

}   // namespace ddge::exec::accessors
