module;

#include <vulkan/vulkan.hpp>

#include <glm/ext/vector_float3.hpp>

export module demos.virtual_texture.VirtualTexture;

import ddge.modules.gfx.resources.VirtualImage;
import ddge.modules.renderer.base.allocator.Allocator;
import ddge.modules.renderer.base.device.Device;
import ddge.modules.renderer.resources.Buffer;

import examples.base.init;

namespace demo {

export class VirtualTexture {
public:
    VirtualTexture(
        const ddge::renderer::base::Device&    device,
        const ddge::renderer::base::Allocator& allocator
    );

    [[nodiscard]]
    auto position() const noexcept -> const glm::vec3&;
    [[nodiscard]]
    auto debug_position() const noexcept -> const glm::vec3&;

    [[nodiscard]]
    auto get() noexcept -> ddge::gfx::resources::VirtualImage&;
    [[nodiscard]]
    auto get() const noexcept -> const ddge::gfx::resources::VirtualImage&;

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
    std::reference_wrapper<const ddge::renderer::base::Device>    m_device_ref;
    std::reference_wrapper<const ddge::renderer::base::Allocator> m_allocator_ref;
    glm::vec3                                                     m_position;
    glm::vec3                                                     m_debug_position;
    ddge::renderer::resources::Buffer                             m_vertex_buffer;
    ddge::renderer::resources::Buffer                             m_debug_vertex_buffer;
    ddge::renderer::resources::Buffer                             m_index_buffer;
    ddge::gfx::resources::VirtualImage                            m_virtual_image;
    vk::UniqueSampler                                             m_virtual_image_sampler;
    vk::UniqueSampler m_virtual_image_debug_sampler;
};

}   // namespace demo
