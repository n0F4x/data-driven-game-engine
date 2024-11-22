module;

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

export module core.renderer.resources.Image;

import core.renderer.base.allocator.Allocator;
import core.renderer.base.resources.Allocation;
import core.renderer.base.resources.Image;

namespace core::renderer::resources {

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

}   // namespace core::renderer::resources
