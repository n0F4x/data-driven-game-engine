module;

#include <cstdint>
#include <limits>

export module modules.renderer.base.resources.MemoryTypeIndex;

namespace modules::renderer::base {

// This is what Vulkan uses for indexing memoryType
export using MemoryTypeIndex = uint32_t;

export constexpr inline MemoryTypeIndex invalid_memory_type_index_v =
    std::numeric_limits<MemoryTypeIndex>::max();

}   // namespace modules::renderer::base
