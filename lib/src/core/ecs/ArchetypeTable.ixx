module;

#include <unordered_map>

export module core.ecs:ArchetypeTable;

import :ArchetypeID;
import :LookupTable;

using ArchetypeTable = std::unordered_map<ArchetypeID, LookupTable>;
