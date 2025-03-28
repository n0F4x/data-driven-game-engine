module;

#include <unordered_map>

export module core.ecs:ComponentTable;

import :ArchetypeID;
import :ErasedComponentContainer;

using ComponentTable = std::unordered_map<ArchetypeID, ErasedComponentContainer>;
