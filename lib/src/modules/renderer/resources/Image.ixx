module;

#include <vk_mem_alloc.h>

export module modules.renderer.resources.Image;

import vulkan_hpp;

import modules.renderer.base.allocator.Allocator;
import modules.renderer.base.resources.Allocation;
import modules.renderer.base.resources.Image;

namespace modules::renderer::resources {

export class Image : public base::Image {
public:
    Image(
        const base::Allocator&         allocator,
        const vk::ImageCreateInfo&     image_create_info,
        const VmaAllocationCreateInfo& allocation_create_info
    );

    auto reset() -> void;

    [[nodiscard]]
    auto allocation() const noexcept -> const base::Allocation&;

private:
    base::Allocation m_allocation;

    [[nodiscard]]
    static auto make(
        const base::Allocator&         allocator,
        const vk::ImageCreateInfo&     image_create_info,
        const VmaAllocationCreateInfo& allocation_create_info
    ) -> Image;

    Image(base::Image&& image, base::Allocation&& allocation) noexcept;
};

}   // namespace modules::renderer::resources
