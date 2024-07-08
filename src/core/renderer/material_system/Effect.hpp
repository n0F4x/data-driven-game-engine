#pragma once

#include <filesystem>

#include "core/cache/Handle.hpp"

#include "Shader.hpp"

namespace core::renderer {

class Effect {
public:
    explicit Effect(Shader t_vertex_shader, Shader t_fragment_shader) noexcept;

    [[nodiscard]]
    auto vertex_shader() const noexcept -> const Shader&;
    [[nodiscard]]
    auto fragment_shader() const noexcept -> const Shader&;

    [[nodiscard]]
    auto pipeline_stages() const -> const std::array<vk::PipelineShaderStageCreateInfo, 2>&;

private:
    Shader                                           m_vertex_shader;
    Shader                                           m_fragment_shader;
    std::array<vk::PipelineShaderStageCreateInfo, 2> m_stages;

    friend auto hash_value(const Effect& t_effect) noexcept -> size_t;
};

}   // namespace core::renderer

template <>
struct std::hash<core::renderer::Effect> {
    [[nodiscard]]
    auto operator()(const core::renderer::Effect& t_effect) const noexcept -> size_t;
};
