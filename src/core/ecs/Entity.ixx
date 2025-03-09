module;

#include <cstdint>

export module core.ecs:Entity;

import utility.Strong;

import :RegistryTag;

namespace core::ecs {

export template <typename Registry_T>
using ID = ::util::Strong<uint64_t, RegistryTag<Registry_T>{}>;

}   // namespace core::ecs

using Key   = ::util::Strong<uint64_t>;
using Index = ::util::Strong<uint32_t>;
