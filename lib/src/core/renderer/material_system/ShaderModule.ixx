module;

#include <filesystem>
#include <optional>

export module core.renderer.material_system.ShaderModule;

import vulkan_hpp;

namespace core::renderer {

export class ShaderModule {
public:
    [[nodiscard]]
    static auto hash(const std::filesystem::path& filepath) noexcept -> size_t;

    [[nodiscard]]
    static auto load(vk::Device device, const std::filesystem::path& filepath)
        -> ShaderModule;

    explicit ShaderModule(
        std::filesystem::path    filepath,
        vk::UniqueShaderModule&& module
    ) noexcept;

    [[nodiscard]]
    auto filepath() const noexcept -> const std::filesystem::path&;
    [[nodiscard]]
    auto module() const noexcept -> vk::ShaderModule;

private:
    std::filesystem::path  m_filepath;
    vk::UniqueShaderModule m_module;

    friend auto hash_value(const ShaderModule& shader_module) noexcept -> size_t;
};

}   // namespace core::renderer

export template <>
struct std::hash<core::renderer::ShaderModule> {
    [[nodiscard]]
    auto operator()(const core::renderer::ShaderModule& shader_module) const noexcept
        -> size_t;
};   // namespace std
