#include "VertexInputStateBuilder.hpp"

#include "VertexInputState.hpp"

namespace engine::renderer {

VertexInputStateBuilder::Binding::Binding(uint32_t t_stride) noexcept
    : m_stride{ t_stride }
{}

auto VertexInputStateBuilder::Binding::stride() const noexcept -> uint32_t
{
    return m_stride;
}

auto VertexInputStateBuilder::Binding::attributes() const noexcept
    -> const VertexInputStateBuilder::Binding::Attributes&
{
    return m_attributes;
}

auto VertexInputStateBuilder::Binding::set_attribute(
    VertexAttribute                               t_attribute,
    const VertexInputStateBuilder::AttributeInfo& t_info
) noexcept -> void
{
    m_attributes[static_cast<size_t>(std::to_underlying(t_attribute))] = t_info;
}

auto VertexInputStateBuilder::Binding::unset_attribute(VertexAttribute t_attribute
) noexcept -> void
{
    m_attributes[static_cast<size_t>(std::to_underlying(t_attribute))] = tl::nullopt;
}

auto VertexInputStateBuilder::bindings() const noexcept -> const std::vector<Binding>&
{
    return m_bindings;
}

auto VertexInputStateBuilder::add_binding(VertexInputStateBuilder::Binding t_binding)
    -> VertexInputStateBuilder&
{
    m_bindings.push_back(std::move(t_binding));
    return *this;
}

auto VertexInputStateBuilder::clear() noexcept -> VertexInputStateBuilder&
{
    m_bindings.clear();
    return *this;
}

auto VertexInputStateBuilder::build(const Effect& t_effect) const noexcept
    -> VertexInputState
{
    return VertexInputState{ t_effect, *this };
}

}   // namespace engine::renderer
