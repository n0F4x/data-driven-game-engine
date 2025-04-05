module;

#include <unordered_map>

export module core.ecs:LookupTableMap;

import :ArchetypeID;
import :LookupTable;

using LookupTableMap = std::unordered_map<ArchetypeID, LookupTable>;
