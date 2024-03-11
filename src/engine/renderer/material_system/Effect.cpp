#include "Effect.hpp"

#include "engine/utility/hashing.hpp"

namespace engine::renderer {

Effect::Effect(Handle<Shader> t_vertex_shader, Handle<Shader> t_fragment_shader) noexcept
    : m_vertex_shader{ std::move(t_vertex_shader) },
      m_fragment_shader{ std::move(t_fragment_shader) },
      m_stages{ vk::PipelineShaderStageCreateInfo{   .stage  = vk::ShaderStageFlagBits::eVertex,
                                                   .module = m_vertex_shader->module(),
                                                   .pName  = m_vertex_shader->entry_point().c_str() },
                vk::PipelineShaderStageCreateInfo{ .stage  = vk::ShaderStageFlagBits::eFragment,
                                                   .module = m_fragment_shader->module(),
                                                   .pName  = m_fragment_shader->entry_point().c_str() } }
{}

auto Effect::vertex_shader() const noexcept -> const Shader&
{
    return *m_vertex_shader;
}

auto Effect::fragment_shader() const noexcept -> const Shader&
{
    return *m_fragment_shader;
}

auto Effect::pipeline_stages() const -> std::span<const vk::PipelineShaderStageCreateInfo>
{
    return m_stages;
}

[[nodiscard]] auto hash_value(const Effect& t_shader) noexcept -> size_t
{
    return hash_combine(*t_shader.m_vertex_shader, *t_shader.m_fragment_shader);
}

}   // namespace engine::renderer

namespace std {

auto hash<engine::renderer::Effect>::operator()(const engine::renderer::Effect& t_effect
) const -> size_t
{
    return engine::renderer::hash_value(t_effect);
}

}   // namespace std
