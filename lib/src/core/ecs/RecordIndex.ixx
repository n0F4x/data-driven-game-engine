module;

#include <cstdint>

export module core.ecs:RecordIndex;

import utility.Strong;

using RecordIndex = ::util::Strong<uint32_t>;
