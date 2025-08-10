module;

#include <cstdint>

export module ddge.modules.ecs:RecordIndex;

import ddge.utility.Strong;

struct RecordIndex : ddge::util::Strong<uint32_t, RecordIndex> {
    using Strong::Strong;
};
