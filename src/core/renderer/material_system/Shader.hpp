#pragma once

#include "core/cache/Handle.hpp"

#include "ShaderModule.hpp"

namespace core::renderer {

class Shader {
public:
    explicit Shader(
        const cache::Handle<ShaderModule>& shader_module,
        std::string                        entry_point
    ) noexcept;

    [[nodiscard]]
    auto filepath() const noexcept -> const std::filesystem::path&;
    [[nodiscard]]
    auto module() const noexcept -> vk::ShaderModule;
    [[nodiscard]]
    auto entry_point() const noexcept -> const std::string&;

private:
    cache::Handle<ShaderModule> m_module;
    std::string                 m_entry_point;

    friend auto hash_value(const Shader& shader) noexcept -> size_t;
};

}   // namespace core::renderer

template <>
struct std::hash<core::renderer::Shader> {
    [[nodiscard]]
    auto operator()(const core::renderer::Shader& shader) const noexcept -> size_t;
};
