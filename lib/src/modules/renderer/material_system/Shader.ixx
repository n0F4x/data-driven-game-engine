module;

#include <filesystem>

export module modules.renderer.material_system.Shader;

import vulkan_hpp;

import modules.cache.Handle;

import modules.renderer.material_system.ShaderModule;

namespace modules::renderer {

export class Shader {
public:
    explicit Shader(
        cache::Handle<const ShaderModule> shader_module,
        std::string                       entry_point = "main"
    );

    [[nodiscard]]
    auto filepath() const noexcept -> const std::filesystem::path&;
    [[nodiscard]]
    auto module() const noexcept -> vk::ShaderModule;
    [[nodiscard]]
    auto entry_point() const noexcept -> const std::string&;

private:
    cache::Handle<const ShaderModule> m_module;
    std::string                       m_entry_point;

    friend auto hash_value(const Shader& shader) noexcept -> std::size_t;
};

}   // namespace modules::renderer

export template <>
struct std::hash<modules::renderer::Shader> {
    [[nodiscard]]
    auto operator()(const modules::renderer::Shader& shader) const noexcept -> std::size_t;
};
