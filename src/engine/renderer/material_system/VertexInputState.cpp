#include "VertexInputState.hpp"

#include <ranges>

#include <vulkan/vulkan_hash.hpp>

#include "engine/utility/hashing.hpp"

#include "VertexAttribute.hpp"
#include "VertexInputStateBuilder.hpp"

namespace engine::renderer {

VertexInputState::VertexInputState(
    const Effect&                  t_effect,
    const VertexInputStateBuilder& t_builder
) noexcept
{
    m_bindings.reserve(t_builder.bindings().size());

    for (size_t binding : std::views::iota(0u, t_builder.bindings().size())) {
        m_bindings.push_back(vk::VertexInputBindingDescription{
            .binding   = static_cast<uint32_t>(binding),
            .stride    = t_builder.bindings()[binding].stride(),
            .inputRate = vk::VertexInputRate::eVertex });
        for (size_t attribute : std::views::iota(
                 0u, static_cast<size_t>(std::to_underlying(VertexAttribute::COUNT))
             ))
        {
            if (t_builder.bindings()[binding].attributes()[attribute].has_value()
                && t_effect.input_attribute_locations()
                       .get(static_cast<VertexAttribute>(attribute))
                       .has_value())
            {
                auto location = t_effect.input_attribute_locations()
                                    .get(static_cast<VertexAttribute>(attribute))
                                    .value();
                auto [format, offset] =
                    t_builder.bindings()[binding].attributes()[attribute].value();

                m_attributes.push_back(vk::VertexInputAttributeDescription{
                    .location = location,
                    .binding  = static_cast<uint32_t>(binding),
                    .format   = format,
                    .offset   = offset });
            }
        }
    }

    m_state = vk::PipelineVertexInputStateCreateInfo{
        .vertexBindingDescriptionCount   = static_cast<uint32_t>(m_bindings.size()),
        .pVertexBindingDescriptions      = m_bindings.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributes.size()),
        .pVertexAttributeDescriptions    = m_attributes.data()
    };
}

auto VertexInputState::bindings() const noexcept
    -> const std::vector<vk::VertexInputBindingDescription>&
{
    return m_bindings;
}

auto VertexInputState::attributes() const noexcept
    -> const std::vector<vk::VertexInputAttributeDescription>&
{
    return m_attributes;
}

auto VertexInputState::info() const noexcept
    -> const vk::PipelineVertexInputStateCreateInfo&
{
    return m_state;
}

[[nodiscard]] auto hash_value(const VertexInputState& t_vertex_input_layout) noexcept
    -> size_t
{
    return hash_combine(
        hash_range(t_vertex_input_layout.m_attributes),
        hash_range(t_vertex_input_layout.m_bindings)
    );
}

}   // namespace engine::renderer

namespace std {

auto hash<engine::renderer::VertexInputState>::operator()(
    const engine::renderer::VertexInputState& t_vertex_input_layout
) const -> size_t
{
    return engine::renderer::hash_value(t_vertex_input_layout);
}

}   // namespace std
