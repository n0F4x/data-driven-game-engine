module;

#include <VkBootstrap.h>

export module ddge.deprecated.gfx.resources.Image;

import vulkan_hpp;

import ddge.deprecated.image.Image;

import ddge.deprecated.renderer.base.allocator.Allocator;
import ddge.deprecated.renderer.base.resources.Image;
import ddge.deprecated.renderer.resources.Image;
import ddge.deprecated.renderer.resources.SeqWriteBuffer;

namespace ddge::gfx::resources {

export class Image {
public:
    class Loader {
    public:
        Loader(
            vk::Device                             device,
            const ddge::renderer::base::Allocator& allocator,
            const ddge::image::Image&              source
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

}   // namespace ddge::gfx::resources
