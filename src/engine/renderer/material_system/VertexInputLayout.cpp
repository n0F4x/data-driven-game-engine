#include "VertexInputLayout.hpp"

#include <ranges>

#include <vulkan/vulkan_format_traits.hpp>
#include <vulkan/vulkan_hash.hpp>

#include "engine/utility/hashing.hpp"

#include "VertexAttribute.hpp"
#include "VertexInputLayoutBuilder.hpp"

namespace engine::renderer {

VertexInputLayout::VertexInputLayout(
    const Effect& t_effect,
    const VertexInputLayoutBuilder& t_builder
) noexcept
{
    for (size_t attribute : std::views::iota(
             0u, static_cast<size_t>(std::to_underlying(VertexAttribute::COUNT))
         ))
    {
        if (t_builder.attributes()[attribute].has_value()
            && t_effect.input_attribute_locations()
                   .get(static_cast<VertexAttribute>(attribute))
                   .has_value())
        {
            auto location = t_effect.input_attribute_locations()
                                .get(static_cast<VertexAttribute>(attribute))
                                .value();
            auto [binding, format, offset] = t_builder.attributes()[attribute].value();

            while (m_bindings.size() < binding + 1) {
                m_bindings.push_back(vk::VertexInputBindingDescription{
                    .binding   = static_cast<uint32_t>(m_bindings.size() - 1),
                    .stride    = {},
                    .inputRate = vk::VertexInputRate::eVertex });
            }
            m_bindings[binding].stride += vk::blockSize(format);

            m_attributes.push_back(vk::VertexInputAttributeDescription{
                .location = location,
                .binding  = binding,
                .format   = format,
                .offset   = offset });
        }
    }

    m_state = vk::PipelineVertexInputStateCreateInfo{
        .vertexBindingDescriptionCount   = static_cast<uint32_t>(m_bindings.size()),
        .pVertexBindingDescriptions      = m_bindings.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributes.size()),
        .pVertexAttributeDescriptions    = m_attributes.data()
    };
}

auto VertexInputLayout::bindings() const noexcept
    -> const std::vector<vk::VertexInputBindingDescription>&
{
    return m_bindings;
}

auto VertexInputLayout::attributes() const noexcept
    -> const std::vector<vk::VertexInputAttributeDescription>&
{
    return m_attributes;
}

auto VertexInputLayout::info() const noexcept
    -> const vk::PipelineVertexInputStateCreateInfo&
{
    return m_state;
}

[[nodiscard]] auto hash_value(const VertexInputLayout& t_vertex_input_layout) noexcept
    -> size_t
{
    return hash_combine(
        hash_range(t_vertex_input_layout.m_attributes),
        hash_range(t_vertex_input_layout.m_bindings)
    );
}

}   // namespace engine::renderer

namespace std {

auto hash<engine::renderer::VertexInputLayout>::operator()(
    const engine::renderer::VertexInputLayout& t_vertex_input_layout
) const -> size_t
{
    return engine::renderer::hash_value(t_vertex_input_layout);
}

}   // namespace std
