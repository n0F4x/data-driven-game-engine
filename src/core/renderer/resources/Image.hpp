#pragma once

#include <vulkan/vulkan.hpp>

#include "core/renderer/base/resources/Allocation.hpp"
#include "core/renderer/base/resources/Image.hpp"

namespace core::renderer::base {

class Allocator;

}   // namespace core::renderer::base

namespace core::renderer::resources {

class Image : public base::Image {
public:
    Image() = default;
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
