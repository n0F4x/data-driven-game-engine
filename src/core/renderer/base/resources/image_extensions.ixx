module;

#include <vulkan/vulkan.hpp>

export module core.renderer.base.resources.image_extensions;

import core.renderer.base.resources.Image;

namespace core::renderer::base {

export [[nodiscard]]
auto ext_memory_requirements(const Image& image) -> vk::MemoryRequirements;

export auto ext_sparse_memory_requirements(const Image& image)
    -> std::vector<vk::SparseImageMemoryRequirements>;

}   // namespace core::renderer::base
