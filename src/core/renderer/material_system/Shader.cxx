module;

#include <filesystem>



module core.renderer.material_system.Shader;

import utility.hashing;

using namespace core::renderer;

namespace core::renderer {

Shader::Shader(
    cache::Handle<const ShaderModule> shader_module,
    std::string                       entry_point
)
    : m_module{ std::move(shader_module) },
      m_entry_point{ std::move(entry_point) }
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
auto hash_value(const Shader& shader) noexcept -> size_t
{
    return hash_combine(*shader.m_module, shader.m_entry_point);
}

}   // namespace core::renderer

auto std::hash<Shader>::operator()(const Shader& shader) const noexcept -> size_t
{
    return hash_value(shader);
}
