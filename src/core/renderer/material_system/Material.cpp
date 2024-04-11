#include "Material.hpp"

namespace core::renderer {

Material::Material(cache::Handle<GraphicsPipeline> t_pipeline) noexcept
    : m_pipeline{ std::move(t_pipeline) }
{}

auto Material::bind(const vk::CommandBuffer t_graphics_command_buffer) const -> void
{
    t_graphics_command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, m_pipeline->get()
    );
}

}   // namespace core::renderer
