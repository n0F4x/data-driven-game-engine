module;

#include <cstdint>
#include <vector>

export module ddge.modules.gfx.resources.virtual_image_helpers;

import vulkan_hpp;

import ddge.modules.image.Image;

import ddge.modules.gfx.resources.VirtualImage;

import ddge.modules.renderer.base.allocator.Allocator;
import ddge.modules.renderer.base.resources.Image;
import ddge.modules.renderer.resources.SeqWriteBuffer;

namespace ddge::gfx::resources {

export auto sparse_color_requirements(const ddge::renderer::base::Image& image)
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
    const ddge::renderer::base::Allocator&   allocator,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> VirtualImage::MipTailRegion;

export [[nodiscard]]
auto stage_tail(
    const ddge::renderer::base::Allocator&   allocator,
    const ddge::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> renderer::resources::SeqWriteBuffer<>;

export [[nodiscard]]
auto create_mip_tail_copy_regions(
    const ddge::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> std::vector<vk::BufferImageCopy>;

}   // namespace ddge::gfx::resources
