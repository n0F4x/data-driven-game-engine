module;

#include <cstdint>

export module ddge.modules.scheduler.data_structures.WorkIndex;

import ddge.utility.containers.Strong;

namespace ddge::scheduler {

export struct WorkIndex : util::Strong<uint64_t, WorkIndex> {
    using Strong::Strong;
};

}   // namespace ddge::scheduler
