module;

#include <vulkan/vulkan.hpp>

#include <glm/ext/vector_float3.hpp>

export module demos.virtual_texture.VirtualTexture;

import modules.gfx.resources.VirtualImage;
import modules.renderer.base.allocator.Allocator;
import modules.renderer.base.device.Device;
import modules.renderer.resources.Buffer;

import examples.base.init;

namespace demo {

export class VirtualTexture {
public:
    VirtualTexture(
        const modules::renderer::base::Device&    device,
        const modules::renderer::base::Allocator& allocator
    );

    [[nodiscard]]
    auto position() const noexcept -> const glm::vec3&;
    [[nodiscard]]
    auto debug_position() const noexcept -> const glm::vec3&;

    [[nodiscard]]
    auto get() noexcept -> modules::gfx::resources::VirtualImage&;
    [[nodiscard]]
    auto get() const noexcept -> const modules::gfx::resources::VirtualImage&;

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
    std::reference_wrapper<const modules::renderer::base::Device>    m_device_ref;
    std::reference_wrapper<const modules::renderer::base::Allocator> m_allocator_ref;
    glm::vec3                                                     m_position;
    glm::vec3                                                     m_debug_position;
    modules::renderer::resources::Buffer                             m_vertex_buffer;
    modules::renderer::resources::Buffer                             m_debug_vertex_buffer;
    modules::renderer::resources::Buffer                             m_index_buffer;
    modules::gfx::resources::VirtualImage                            m_virtual_image;
    vk::UniqueSampler                                             m_virtual_image_sampler;
    vk::UniqueSampler m_virtual_image_debug_sampler;
};

}   // namespace demo
