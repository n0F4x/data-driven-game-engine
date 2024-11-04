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

[[nodiscard]]
auto stage_tail(
    const core::renderer::base::Allocator&   allocator,
    const core::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements,
    const VirtualImage::MipTailRegion&       tail
) -> core::renderer::resources::SeqWriteBuffer<>;

[[nodiscard]]
auto create_mip_tail_copy_regions(
    const core::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> std::vector<vk::BufferImageCopy>;

}   // namespace core::gfx::resources
