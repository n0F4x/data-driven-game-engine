#include "Effect.hpp"

#include "engine/utility/hashing.hpp"

namespace engine::renderer {

Effect::Effect(Handle<Shader> t_vertex_shader, Handle<Shader> t_fragment_shader) noexcept
    : m_vertex_shader{ std::move(t_vertex_shader) },
      m_fragment_shader{ std::move(t_fragment_shader) }
{}

auto Effect::vertex_shader() const noexcept -> const Shader&
{
    return *m_vertex_shader;
}

auto Effect::fragment_shader() const noexcept -> const Shader&
{
    return *m_fragment_shader;
}

[[nodiscard]] auto hash_value(const Effect& t_shader) noexcept -> size_t
{
    return hash_combine(*t_shader.m_vertex_shader, *t_shader.m_fragment_shader);
}

}   // namespace engine::renderer
