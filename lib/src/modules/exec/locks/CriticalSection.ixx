module;

#include <variant>

export module ddge.modules.exec.locks.CriticalSection;

import ddge.modules.exec.locks.ExclusiveCriticalSection;
import ddge.modules.exec.locks.SharedCriticalSection;

namespace ddge::exec {

export using CriticalSection =
    std::variant<ExclusiveCriticalSection, SharedCriticalSection>;

}   // namespace ddge::exec
