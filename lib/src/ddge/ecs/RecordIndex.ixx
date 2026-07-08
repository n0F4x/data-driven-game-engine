module;

#include <cstdint>

export module ddge.ecs:RecordIndex;

import ddge.util.containers.Strong;

struct RecordIndex : ddge::util::Strong<uint32_t, RecordIndex> {
    using Strong::Strong;
};
