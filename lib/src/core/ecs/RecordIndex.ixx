module;

#include <cstdint>

export module core.ecs:RecordIndex;

import utility.Strong;

struct RecordIndex : util::Strong<uint32_t, RecordIndex> {
    using Strong::Strong;
};
