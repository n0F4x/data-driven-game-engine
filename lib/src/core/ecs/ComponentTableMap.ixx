module;

#include <map>

export module core.ecs:ComponentTableMap;

import :ComponentID;
import :ComponentTable;

using ComponentTableMap = std::map<ComponentID, ComponentTable>;
