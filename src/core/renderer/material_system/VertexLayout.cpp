#include "VertexLayout.hpp"

namespace core::renderer {

VertexLayout::VertexLayout(
    const uint32_t            stride,
    const vk::VertexInputRate input_rate
) noexcept
    : m_stride{ stride },
      m_input_rate{ input_rate }
{}

auto VertexLayout::stride() const noexcept -> uint32_t
{
    return m_stride;
}

auto VertexLayout::input_rate() const noexcept -> vk::VertexInputRate
{
    return m_input_rate;
}

auto VertexLayout::attributes() const noexcept -> std::span<const VertexAttribute>
{
    return m_attributes;
}

}   // namespace core::renderer
