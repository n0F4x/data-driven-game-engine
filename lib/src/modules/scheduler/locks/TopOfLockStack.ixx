module;

#include <unordered_map>

export module ddge.modules.scheduler.locks.TopOfLockStack;

import ddge.modules.scheduler.locks.CriticalSection;
import ddge.modules.scheduler.locks.LockedResourceID;

namespace ddge::scheduler {

export using TopOfLockStack = std::unordered_map<LockedResourceID, CriticalSection>;

}   // namespace ddge::scheduler
