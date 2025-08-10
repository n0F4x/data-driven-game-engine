module;

#include <filesystem>


module ddge.modules.renderer.material_system.Shader;

import ddge.utility.hashing;

using namespace ddge::renderer;

namespace ddge::renderer {

Shader::Shader(cache::Handle<const ShaderModule> shader_module, std::string entry_point)
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
auto hash_value(const Shader& shader) noexcept -> std::size_t
{
    return util::hash_combine(*shader.m_module, shader.m_entry_point);
}

}   // namespace ddge::renderer

auto std::hash<Shader>::operator()(const Shader& shader) const noexcept -> std::size_t
{
    return hash_value(shader);
}
