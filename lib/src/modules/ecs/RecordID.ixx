module;

#include <cstdint>

export module modules.ecs:RecordID;

import utility.Strong;

struct RecordID : util::Strong<uint64_t, RecordID> {
    using Strong::Strong;
};
