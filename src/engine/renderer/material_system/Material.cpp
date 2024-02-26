#include "Material.hpp"

Material::Material(const vk::Pipeline t_pipeline) noexcept : m_pipeline{ t_pipeline } {}

auto Material::bind(const vk::CommandBuffer t_graphics_command_buffer) const -> void
{
    t_graphics_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
}
