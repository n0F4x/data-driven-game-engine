module;

#include <variant>

export module ddge.scheduler.locks.CriticalSection;

import ddge.scheduler.locks.ExclusiveCriticalSection;
import ddge.scheduler.locks.SharedCriticalSection;

namespace ddge::scheduler {

export using CriticalSection =
    std::variant<ExclusiveCriticalSection, SharedCriticalSection>;

}   // namespace ddge::scheduler
