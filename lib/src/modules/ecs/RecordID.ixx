module;

#include <cstdint>

export module ddge.modules.ecs:RecordID;

import ddge.utility.Strong;

struct RecordID : ddge::util::Strong<uint64_t, RecordID> {
    using Strong::Strong;
};
