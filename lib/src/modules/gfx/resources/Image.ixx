module;

#include <VkBootstrap.h>

export module modules.gfx.resources.Image;

import vulkan_hpp;

import modules.image.Image;

import modules.renderer.base.allocator.Allocator;
import modules.renderer.base.resources.Image;
import modules.renderer.resources.Image;
import modules.renderer.resources.SeqWriteBuffer;

namespace modules::gfx::resources {

export class Image {
public:
    class Loader {
    public:
        Loader(
            vk::Device                             device,
            const modules::renderer::base::Allocator& allocator,
            const modules::image::Image&              source
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

}   // namespace modules::gfx::resources
