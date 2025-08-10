module;

#include <ranges>

#include <vk_mem_alloc.h>

module ddge.modules.renderer.resources.Image;

import ddge.modules.renderer.base.allocator.Allocator;

ddge::renderer::resources::Image::Image(
    const base::Allocator&         allocator,
    const vk::ImageCreateInfo&     image_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
)
    : Image{ make(allocator, image_create_info, allocation_create_info) }
{}

auto ddge::renderer::resources::Image::reset() -> void
{
    m_allocation.reset();
    base::Image::reset();
}

auto ddge::renderer::resources::Image::allocation() const noexcept
    -> const base::Allocation&
{
    return m_allocation;
}

auto ddge::renderer::resources::Image::make(
    const base::Allocator&         allocator,
    const vk::ImageCreateInfo&     image_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
) -> Image
{
    auto [image, allocation, _]{
        allocator.create_image(image_create_info, allocation_create_info)
    };

    return Image{ std::move(image), std::move(allocation) };
}

ddge::renderer::resources::Image::Image(
    base::Image&&      image,
    base::Allocation&& allocation
) noexcept
    : base::Image{ std::move(image) },
      m_allocation{ std::move(allocation) }
{}
