module;

#include <cstdint>

export module ddge.ecs:RecordID;

import ddge.util.containers.Strong;

struct RecordID : ddge::util::Strong<uint64_t, RecordID> {
    using Strong::Strong;
};
