#pragma once

#include <vulkan/vulkan.hpp>

namespace core::renderer::base {

class Image;

}   // namespace core::renderer::base

namespace core::renderer::base {

[[nodiscard]]
auto ext_memory_requirements(const Image& image) -> vk::MemoryRequirements;
auto ext_sparse_memory_requirements(const Image& image)
    -> std::vector<vk::SparseImageMemoryRequirements>;

}   // namespace core::renderer::base
