#include "VertexInputLayoutBuilder.hpp"

#include "VertexInputLayout.hpp"

namespace engine::renderer {

auto VertexInputLayoutBuilder::set_attribute(
    const VertexAttribute t_attribute,
    const AttributeInfo&  t_info
) noexcept -> VertexInputLayoutBuilder&
{
    m_attributes[static_cast<size_t>(std::to_underlying(t_attribute))] = t_info;
    return *this;
}

auto VertexInputLayoutBuilder::attributes() const noexcept
    -> const VertexInputLayoutBuilder::Attributes&
{
    return m_attributes;
}

auto VertexInputLayoutBuilder::unset_attribute(VertexAttribute t_attribute) noexcept
    -> VertexInputLayoutBuilder&
{
    m_attributes[static_cast<size_t>(std::to_underlying(t_attribute))] = tl::nullopt;
    return *this;
}

auto VertexInputLayoutBuilder::build(const ShaderModule& t_vertex_shader) const noexcept
    -> VertexInputLayout
{
    return VertexInputLayout{ t_vertex_shader, *this };
}

}   // namespace engine::renderer
