#pragma once

#include <limits>

namespace core::renderer::base {

// This is what Vulkan uses for indexing memoryType
using MemoryTypeIndex = uint32_t;

constexpr inline MemoryTypeIndex invalid_memory_type_index_v =
    std::numeric_limits<MemoryTypeIndex>::max();

}   // namespace core::renderer::base
