module;

#include <cstdint>

export module core.ecs:RecordID;

import utility.Strong;

using RecordID = ::util::Strong<uint64_t, [] {}>;
