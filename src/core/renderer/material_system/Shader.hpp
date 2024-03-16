#pragma once

#include "core/common/Handle.hpp"

#include "ShaderModule.hpp"

namespace core::renderer {

class Shader {
public:
    explicit Shader(
        Handle<ShaderModule> t_shader_module,
        std::string          t_entry_point
    ) noexcept;

    [[nodiscard]] auto filepath() const noexcept -> const std::filesystem::path&;
    [[nodiscard]] auto module() const noexcept -> vk::ShaderModule;
    [[nodiscard]] auto entry_point() const noexcept -> const std::string&;

private:
    Handle<ShaderModule> m_module;
    std::string          m_entry_point;

    friend auto hash_value(const Shader& t_shader) noexcept -> size_t;
};

}   // namespace core::renderer

namespace std {

template <>
class hash<core::renderer::Shader> {
public:
    [[nodiscard]] auto operator()(const core::renderer::Shader& t_shader) const
        -> size_t;
};

}   // namespace std
