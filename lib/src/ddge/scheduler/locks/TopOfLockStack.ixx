module;

#include <unordered_map>

export module ddge.scheduler.locks.TopOfLockStack;

import ddge.scheduler.locks.CriticalSection;
import ddge.scheduler.locks.LockedResourceID;

namespace ddge::scheduler {

export using TopOfLockStack = std::unordered_map<LockedResourceID, CriticalSection>;

}   // namespace ddge::scheduler
