module;

#include <cstdint>

export module ddge.modules.ecs:RecordID;

import ddge.utility.containers.Strong;

struct RecordID : ddge::util::Strong<uint64_t, RecordID> {
    using Strong::Strong;
};
