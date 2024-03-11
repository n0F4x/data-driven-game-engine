#include "Shader.hpp"

#include "engine/utility/hashing.hpp"

namespace engine::renderer {

Shader::Shader(Handle<ShaderModule> t_shader_module, std::string t_entry_point) noexcept
    : m_module{ std::move(t_shader_module) },
      m_entry_point{ std::move(t_entry_point) }
{}

auto Shader::filepath() const noexcept -> const std::filesystem::path&
{
    return m_module->filepath();
}

auto Shader::module() const noexcept -> vk::ShaderModule
{
    return m_module->module();
}

auto Shader::entry_point() const noexcept -> const std::string&
{
    return m_entry_point;
}

[[nodiscard]] auto hash_value(const Shader& t_shader) noexcept -> size_t
{
    return hash_combine(*t_shader.m_module, t_shader.m_entry_point);
}

}   // namespace engine::renderer

namespace std {

auto hash<engine::renderer::Shader>::operator()(const engine::renderer::Shader& t_shader
) const -> size_t
{
    return engine::renderer::hash_value(t_shader);
}

}   // namespace std
