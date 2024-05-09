#pragma once

#include <filesystem>

#include <optional>

#include <vulkan/vulkan.hpp>

#include "core/cache/Cache.hpp"
#include "core/cache/Handle.hpp"

namespace core::renderer {

class ShaderModule {
public:
    [[nodiscard]]
    static auto hash(const std::filesystem::path& t_filepath) noexcept -> size_t;

    [[nodiscard]]
    static auto create(vk::Device t_device, const std::filesystem::path& t_filepath)
        -> std::optional<ShaderModule>;

    explicit ShaderModule(
        std::filesystem::path    t_filepath,
        vk::UniqueShaderModule&& t_module
    ) noexcept;

    [[nodiscard]]
    auto filepath() const noexcept -> const std::filesystem::path&;
    [[nodiscard]]
    auto module() const noexcept -> vk::ShaderModule;

private:
    std::filesystem::path  m_filepath;
    vk::UniqueShaderModule m_module;

    friend auto hash_value(const ShaderModule& t_shader_module) noexcept -> size_t;
};

}   // namespace core::renderer

template <>
struct std::hash<core::renderer::ShaderModule> {
    [[nodiscard]]
    auto operator()(const core::renderer::ShaderModule& t_shader_module
    ) const noexcept -> size_t;
};   // namespace std
