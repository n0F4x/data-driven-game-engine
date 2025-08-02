module;

#include <cstdint>
#include <limits>

export module core.renderer.base.resources.MemoryTypeIndex;

namespace core::renderer::base {

// This is what Vulkan uses for indexing memoryType
export using MemoryTypeIndex = uint32_t;

export constexpr inline MemoryTypeIndex invalid_memory_type_index_v =
    std::numeric_limits<MemoryTypeIndex>::max();

}   // namespace core::renderer::base
