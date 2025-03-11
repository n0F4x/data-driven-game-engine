module;

#include <filesystem>

export module core.renderer.material_system.Shader;

import vulkan_hpp;

import core.cache.Handle;

import core.renderer.material_system.ShaderModule;

namespace core::renderer {

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

    friend auto hash_value(const Shader& shader) noexcept -> size_t;
};

}   // namespace core::renderer

export template <>
struct std::hash<core::renderer::Shader> {
    [[nodiscard]]
    auto operator()(const core::renderer::Shader& shader) const noexcept -> size_t;
};
