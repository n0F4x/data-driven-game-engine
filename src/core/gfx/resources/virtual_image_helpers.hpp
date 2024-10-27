#pragma once

#include "VirtualImage.hpp"

namespace core::gfx::resources {

auto sparse_color_requirements(const core::renderer::base::Image& image)
    -> vk::SparseImageMemoryRequirements;

[[nodiscard]]
auto create_sparse_blocks(
    const vk::Extent3D&                      extent,
    uint32_t                                 mip_level_count,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> std::vector<VirtualImage::Block>;

[[nodiscard]]
auto create_mip_tail_region(
    const core::renderer::base::Allocator&   allocator,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> VirtualImage::MipTailRegion;

}   // namespace core::gfx::resources
