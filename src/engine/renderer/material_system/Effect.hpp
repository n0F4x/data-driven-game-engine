#pragma once

#include "engine/common/Handle.hpp"

#include "Shader.hpp"

namespace engine::renderer {

class Effect {
public:
    explicit Effect(
        Handle<Shader> t_vertex_shader,
        Handle<Shader> t_fragment_shader
    ) noexcept;

    [[nodiscard]] auto vertex_shader() const noexcept -> const Shader&;
    [[nodiscard]] auto fragment_shader() const noexcept -> const Shader&;

    [[nodiscard]] auto pipeline_stages() const
        -> std::span<const vk::PipelineShaderStageCreateInfo>;

private:
    Handle<Shader>                                   m_vertex_shader;
    Handle<Shader>                                   m_fragment_shader;
    std::array<vk::PipelineShaderStageCreateInfo, 2> m_stages;

    friend auto hash_value(const Effect& t_shader) noexcept -> size_t;
};

}   // namespace engine::renderer

namespace std {

template <>
class hash<engine::renderer::Effect> {
public:
    [[nodiscard]] auto operator()(const engine::renderer::Effect& t_effect) const
        -> size_t;
};

}   // namespace std
