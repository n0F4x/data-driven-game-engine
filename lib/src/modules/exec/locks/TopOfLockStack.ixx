module;

#include <unordered_map>

export module ddge.modules.exec.locks.TopOfLockStack;

import ddge.modules.exec.locks.CriticalSection;
import ddge.modules.exec.locks.LockedResourceID;

namespace ddge::exec {

export using TopOfLockStack = std::unordered_map<LockedResourceID, CriticalSection>;

}   // namespace ddge::exec
