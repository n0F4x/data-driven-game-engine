#pragma once

#include <filesystem>

#include "core/cache/Handle.hpp"

#include "Shader.hpp"
#include "VertexAttribute.hpp"

namespace core::renderer {

class Effect {
public:
    class InputAttributeLocations {
    public:
        [[nodiscard]] static auto
            load_from_shader_file(const std::filesystem::path& t_filepath)
                -> InputAttributeLocations;

        [[nodiscard]] auto get(VertexAttribute t_attribute) const noexcept
            -> tl::optional<uint32_t>;

    private:
        std::array<tl::optional<uint32_t>, static_cast<size_t>(VertexAttribute::COUNT)>
            m_locations{};
    };

    explicit Effect(
        const cache::Handle<Shader>& t_vertex_shader,
        const cache::Handle<Shader>& t_fragment_shader
    ) noexcept;

    [[nodiscard]] auto vertex_shader() const noexcept -> const Shader&;
    [[nodiscard]] auto fragment_shader() const noexcept -> const Shader&;
    [[nodiscard]] auto input_attribute_locations() const noexcept
        -> const InputAttributeLocations&;

    [[nodiscard]] auto pipeline_stages() const
        -> std::span<const vk::PipelineShaderStageCreateInfo>;

private:
    cache::Handle<Shader>                                   m_vertex_shader;
    cache::Handle<Shader>                                   m_fragment_shader;
    std::array<vk::PipelineShaderStageCreateInfo, 2> m_stages;
    InputAttributeLocations                          m_attribute_locations;

    friend auto hash_value(const Effect& t_effect) noexcept -> size_t;
};

}   // namespace core::renderer

template <>
struct std::hash<core::renderer::Effect> {
    [[nodiscard]] auto operator()(const core::renderer::Effect& t_effect) const noexcept
        -> size_t;
};
