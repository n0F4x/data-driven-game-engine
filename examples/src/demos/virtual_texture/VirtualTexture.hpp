#pragma once

#include <core/gfx/resources/VirtualImage.hpp>
#include <core/renderer/resources/Buffer.hpp>

namespace demo {

class VirtualTexture {
public:
    VirtualTexture(
        const core::renderer::base::Device&    device,
        const core::renderer::base::Allocator& allocator
    );

    [[nodiscard]]
    auto get() noexcept -> core::gfx::resources::VirtualImage&;
    [[nodiscard]]
    auto get() const noexcept -> const core::gfx::resources::VirtualImage&;

    [[nodiscard]]
    auto view() const noexcept -> vk::ImageView;
    [[nodiscard]]
    auto debug_view() const noexcept -> vk::ImageView;

    [[nodiscard]]
    auto sampler() const noexcept -> vk::Sampler;
    [[nodiscard]]
    auto debug_sampler() const noexcept -> vk::Sampler;

    auto draw(vk::CommandBuffer command_buffer) -> void;
    auto draw_debug(vk::CommandBuffer command_buffer) const -> void;

private:
    std::reference_wrapper<const core::renderer::base::Device>    m_device_ref;
    std::reference_wrapper<const core::renderer::base::Allocator> m_allocator_ref;
    core::renderer::resources::Buffer                             m_vertex_buffer;
    core::renderer::resources::Buffer                             m_debug_vertex_buffer;
    core::renderer::resources::Buffer                             m_index_buffer;
    core::gfx::resources::VirtualImage                            m_virtual_image;
    vk::UniqueSampler                                             m_virtual_image_sampler;
    vk::UniqueSampler m_virtual_image_debug_sampler;
};

}   // namespace demo
