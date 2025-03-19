module;

#include <cstdint>

export module core.ecs:ID;

import utility.Strong;

import :RegistryTag;

namespace core::ecs {

export template <typename Registry_T>
using ID = ::util::Strong<uint64_t, void, registry_tag<Registry_T>>;

}   // namespace core::ecs
