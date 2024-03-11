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

private:
    Handle<Shader> m_vertex_shader;
    Handle<Shader> m_fragment_shader;

    friend auto hash_value(const Effect& t_shader) noexcept -> size_t;
};

}   // namespace engine::renderer
