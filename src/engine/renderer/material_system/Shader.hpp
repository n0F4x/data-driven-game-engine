#pragma once

#include "engine/common/Handle.hpp"

#include "ShaderModule.hpp"

namespace engine::renderer {

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

}   // namespace engine::renderer

namespace std {

template <>
class hash<engine::renderer::Shader> {
public:
    [[nodiscard]] auto operator()(const engine::renderer::Shader& t_shader) const
        -> size_t;
};

}   // namespace std
