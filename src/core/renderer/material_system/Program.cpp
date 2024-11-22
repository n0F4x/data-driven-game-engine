module;

#include <string_view>



module core.renderer.material_system.Program;

import core.utility.hashing;

using namespace core::renderer;

namespace core::renderer {

Program::Program(Shader vertex_shader, Shader fragment_shader) noexcept
    : m_vertex_shader{ std::move(vertex_shader) },
      m_fragment_shader{ std::move(fragment_shader) }
{}

auto Program::vertex_shader() const noexcept -> const Shader&
{
    return m_vertex_shader;
}

auto Program::fragment_shader() const noexcept -> const Shader&
{
    return m_fragment_shader;
}

auto Program::pipeline_stages() const -> std::array<vk::PipelineShaderStageCreateInfo, 2>
{
    return std::array{
        vk::PipelineShaderStageCreateInfo{
                                          .stage  = vk::ShaderStageFlagBits::eVertex,
                                          .module = m_vertex_shader.module(),
                                          .pName  = m_vertex_shader.entry_point().c_str()  },
        vk::PipelineShaderStageCreateInfo{
                                          .stage  = vk::ShaderStageFlagBits::eFragment,
                                          .module = m_fragment_shader.module(),
                                          .pName  = m_fragment_shader.entry_point().c_str() },
    };
}

[[nodiscard]]
auto hash_value(const Program& program) noexcept -> size_t
{
    return hash_combine(program.m_vertex_shader, program.m_fragment_shader);
}

}   // namespace core::renderer

auto std::hash<Program>::operator()(const Program& program) const noexcept -> size_t
{
    return hash_value(program);
}
