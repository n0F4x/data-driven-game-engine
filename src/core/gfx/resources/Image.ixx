module;

#include <VkBootstrap.h>

export module core.gfx.resources.Image;

import vulkan_hpp;

import core.image.Image;

import core.renderer.base.allocator.Allocator;
import core.renderer.base.resources.Image;
import core.renderer.resources.Image;
import core.renderer.resources.SeqWriteBuffer;

namespace core::gfx::resources {

export class Image {
public:
    class Loader {
    public:
        Loader(
            vk::Device                             device,
            const core::renderer::base::Allocator& allocator,
            const core::image::Image&              source
        );

        [[nodiscard]]
        auto operator()(
            vk::PhysicalDevice                  physical_device,
            vk::CommandBuffer                   graphics_command_buffer,
            const renderer::base::Image::State& new_state
        ) && -> Image;

        [[nodiscard]]
        auto image() const -> const renderer::resources::Image&;

        [[nodiscard]]
        auto view() const -> vk::ImageView;

    private:
        renderer::resources::Image m_image;
        vk::UniqueImageView        m_view;

        std::vector<vk::BufferImageCopy>      m_copy_regions;
        renderer::resources::SeqWriteBuffer<> m_staging_buffer;
        bool                                  m_needs_mip_generation;
    };

    struct Requirements {
        static auto require_device_settings(
            vkb::PhysicalDeviceSelector& physical_device_selector
        ) -> void;
    };

    [[nodiscard]]
    auto view() const -> vk::ImageView;

private:
    renderer::resources::Image m_image;
    vk::UniqueImageView        m_view;

    Image(renderer::resources::Image&& image, vk::UniqueImageView&& view) noexcept;
};

}   // namespace core::gfx::resources
