module;

#include <cstdint>
#include <vector>

export module modules.gfx.resources.virtual_image_helpers;

import vulkan_hpp;

import modules.image.Image;

import modules.gfx.resources.VirtualImage;

import modules.renderer.base.allocator.Allocator;
import modules.renderer.base.resources.Image;
import modules.renderer.resources.SeqWriteBuffer;

namespace modules::gfx::resources {

export auto sparse_color_requirements(const modules::renderer::base::Image& image)
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
    const modules::renderer::base::Allocator&   allocator,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> VirtualImage::MipTailRegion;

export [[nodiscard]]
auto stage_tail(
    const modules::renderer::base::Allocator&   allocator,
    const modules::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> renderer::resources::SeqWriteBuffer<>;

export [[nodiscard]]
auto create_mip_tail_copy_regions(
    const modules::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> std::vector<vk::BufferImageCopy>;

}   // namespace modules::gfx::resources
