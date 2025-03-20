module;

#include <cstdint>

export module core.ecs:RecordID;

import utility.Strong;

// TODO: remove explicit []{} tag when Clang allows it
using RecordID = ::util::Strong<uint64_t, [] {}>;
