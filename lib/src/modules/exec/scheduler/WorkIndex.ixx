module;

#include <cstdint>

export module ddge.modules.exec.scheduler.WorkIndex;

import ddge.utility.containers.Strong;

namespace ddge::exec {

export struct WorkIndex : util::Strong<uint64_t, WorkIndex> {
    using Strong::Strong;
};

}   // namespace ddge::exec
