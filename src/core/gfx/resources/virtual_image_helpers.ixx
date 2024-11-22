module;

#include <cstdint>
#include <vector>

export module core.gfx.resources.virtual_image_helpers;

import vulkan_hpp;

import core.image.Image;

import core.gfx.resources.VirtualImage;

import core.renderer.base.allocator.Allocator;
import core.renderer.base.resources.Image;
import core.renderer.resources.SeqWriteBuffer;

namespace core::gfx::resources {

export auto sparse_color_requirements(const core::renderer::base::Image& image)
    -> vk::SparseImageMemoryRequirements;

export [[nodiscard]]
auto create_sparse_blocks(
    const image::Image&                      source,
    const vk::Extent3D&                      extent,
    uint32_t                                 mip_level_count,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> std::vector<VirtualImage::Block>;

export [[nodiscard]]
auto create_mip_tail_region(
    const core::renderer::base::Allocator&   allocator,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> VirtualImage::MipTailRegion;

export [[nodiscard]]
auto stage_tail(
    const core::renderer::base::Allocator&   allocator,
    const core::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> renderer::resources::SeqWriteBuffer<>;

export [[nodiscard]]
auto create_mip_tail_copy_regions(
    const core::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> std::vector<vk::BufferImageCopy>;

}   // namespace core::gfx::resources
