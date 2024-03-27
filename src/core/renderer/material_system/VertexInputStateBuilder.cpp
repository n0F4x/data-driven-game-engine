#include "VertexInputStateBuilder.hpp"

#include "VertexInputState.hpp"

namespace core::renderer {

auto VertexInputStateBuilder::Binding::Attributes::operator[](
    const VertexAttribute t_attributes
) noexcept -> tl::optional<AttributeInfo>&
{
    assert(t_attributes < VertexAttribute::COUNT);
    return m_attributes[static_cast<size_t>(std::to_underlying(t_attributes))];
}

auto VertexInputStateBuilder::Binding::Attributes::operator[](
    const VertexAttribute t_attributes
) const noexcept -> const tl::optional<AttributeInfo>&
{
    assert(t_attributes < VertexAttribute::COUNT);
    return m_attributes[static_cast<size_t>(std::to_underlying(t_attributes))];
}

VertexInputStateBuilder::Binding::Binding(const uint32_t t_stride) noexcept
    : m_stride{ t_stride }
{}

auto VertexInputStateBuilder::Binding::stride() const noexcept -> uint32_t
{
    return m_stride;
}

auto VertexInputStateBuilder::Binding::attributes() const noexcept -> const Attributes&
{
    return m_attributes;
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

}   // namespace core::renderer
