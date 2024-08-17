#include "Effect.hpp"

#include <string_view>

#include "core/utility/hashing.hpp"

namespace core::renderer {

Effect::Effect(
    Shader vertex_shader,
    Shader fragment_shader) noexcept
    : m_vertex_shader{ std::move(vertex_shader) },
      m_fragment_shader{ std::move(fragment_shader) },
      m_stages{ vk::PipelineShaderStageCreateInfo{   .stage  = vk::ShaderStageFlagBits::eVertex,
                                                   .module = m_vertex_shader.module(),
                                                   .pName  = m_vertex_shader.entry_point().c_str() },
                vk::PipelineShaderStageCreateInfo{ .stage  = vk::ShaderStageFlagBits::eFragment,
                                                   .module = m_fragment_shader.module(),
                                                   .pName  = m_fragment_shader.entry_point().c_str() } }
{}

auto Effect::vertex_shader() const noexcept -> const Shader&
{
    return m_vertex_shader;
}

auto Effect::fragment_shader() const noexcept -> const Shader&
{
    return m_fragment_shader;
}

auto Effect::pipeline_stages() const
    -> const std::array<vk::PipelineShaderStageCreateInfo, 2>&
{
    return m_stages;
}

[[nodiscard]]
auto hash_value(const Effect& effect) noexcept -> size_t
{
    return hash_combine(effect.m_vertex_shader, effect.m_fragment_shader);
}

}   // namespace core::renderer

auto std::hash<core::renderer::Effect>::operator()(const core::renderer::Effect& effect
) const noexcept -> size_t
{
    return core::renderer::hash_value(effect);
}
