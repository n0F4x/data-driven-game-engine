module;

#include <vector>

export module modules.renderer.base.resources.image_extensions;

import vulkan_hpp;

import modules.renderer.base.resources.Image;

namespace modules::renderer::base {

export [[nodiscard]]
auto ext_memory_requirements(const Image& image) -> vk::MemoryRequirements;

export auto ext_sparse_memory_requirements(const Image& image)
    -> std::vector<vk::SparseImageMemoryRequirements>;

}   // namespace modules::renderer::base
