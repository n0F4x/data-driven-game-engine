#pragma once

#include <tl/optional.hpp>

#include "Effect.hpp"
#include "VertexAttribute.hpp"

namespace engine::renderer {

class VertexInputLayout;

class VertexInputLayoutBuilder {
public:
    struct AttributeInfo {
        uint32_t   binding;
        vk::Format format;
        uint32_t   offset;
    };

    using Attributes =
        std::array<tl::optional<AttributeInfo>, static_cast<size_t>(VertexAttribute::COUNT)>;

    [[nodiscard]] auto attributes() const noexcept -> const Attributes&;

    auto set_attribute(VertexAttribute t_attribute, const AttributeInfo& t_info) noexcept
        -> VertexInputLayoutBuilder&;
    auto unset_attribute(VertexAttribute t_attribute) noexcept
        -> VertexInputLayoutBuilder&;

    [[nodiscard]] auto build(const Effect& t_effect) const noexcept
        -> VertexInputLayout;

private:
    Attributes m_attributes;
};

}   // namespace engine::renderer
