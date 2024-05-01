#include "Shader.hpp"

#include "core/utility/hashing.hpp"

namespace core::renderer {

Shader::Shader(
    const cache::Handle<ShaderModule>& t_shader_module,
    std::string                        t_entry_point
) noexcept
    : m_module{ t_shader_module },
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

[[nodiscard]]
auto hash_value(const Shader& t_shader) noexcept -> size_t
{
    return hash_combine(*t_shader.m_module, t_shader.m_entry_point);
}

}   // namespace core::renderer

auto std::hash<core::renderer::Shader>::operator()(const core::renderer::Shader& t_shader
) const noexcept -> size_t
{
    return core::renderer::hash_value(t_shader);
}
