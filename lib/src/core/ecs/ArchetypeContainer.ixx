module;

#include <unordered_map>

module core.ecs:ArchetypeContainer;

import :Archetype;
import :ArchetypeID;

using ArchetypeContainer = std::unordered_map<::ArchetypeID, ::Archetype>;
