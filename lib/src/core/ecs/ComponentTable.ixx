module;

#include <map>

export module core.ecs:ComponentTable;

import :ArchetypeID;
import :ErasedComponentContainer;

using ComponentTable = std::map<ArchetypeID, ErasedComponentContainer>;
