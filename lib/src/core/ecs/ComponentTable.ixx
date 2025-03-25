module;

#include <map>

export module core.ecs:ComponentTable;

import :ArchetypeID;
import :ErasedComponentContainer;

// TODO: make this unordered_map when Query function clears up
using ComponentTable = std::map<ArchetypeID, ErasedComponentContainer>;
