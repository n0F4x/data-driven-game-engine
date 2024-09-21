#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace core::renderer {

struct VertexAttribute { // NOLINT(*-member-init)
    uint32_t   location;
    vk::Format format;
    uint32_t   offset;
};

class VertexLayout {
public:
    explicit VertexLayout(
        uint32_t            stride,
        vk::VertexInputRate input_rate = vk::VertexInputRate::eVertex
    ) noexcept;

    [[nodiscard]]
    auto stride() const noexcept -> uint32_t;
    [[nodiscard]]
    auto input_rate() const noexcept -> vk::VertexInputRate;
    [[nodiscard]]
    auto attributes() const noexcept -> std::span<const VertexAttribute>;

    template <typename Self>
    auto add_attribute(this Self&&, VertexAttribute attribute) -> Self;

private:
    uint32_t                     m_stride;
    vk::VertexInputRate          m_input_rate;
    std::vector<VertexAttribute> m_attributes;
};

}   // namespace core::renderer

#include "VertexLayout.inl"
