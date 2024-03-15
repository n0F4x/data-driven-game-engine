#pragma once

#include <tl/optional.hpp>

#include "Effect.hpp"
#include "VertexAttribute.hpp"

namespace engine::renderer {

class VertexInputState;

class VertexInputStateBuilder {
public:
    struct AttributeInfo {
        vk::Format format;
        uint32_t   offset;
    };

    class Binding {
    public:
        using Attributes = std::array<
            tl::optional<AttributeInfo>,
            static_cast<size_t>(VertexAttribute::COUNT)>;

        explicit Binding(uint32_t t_stride) noexcept;

        [[nodiscard]] auto stride() const noexcept -> uint32_t;
        [[nodiscard]] auto attributes() const noexcept -> const Attributes&;

        auto set_attribute(VertexAttribute t_attribute, const AttributeInfo& t_info) noexcept
            -> void;
        auto unset_attribute(VertexAttribute t_attribute) noexcept -> void;

    private:
        uint32_t   m_stride;
        Attributes m_attributes;
    };

    [[nodiscard]] auto bindings() const noexcept -> const std::vector<Binding>&;

    auto add_binding(Binding t_binding) -> VertexInputStateBuilder&;
    auto clear() noexcept -> VertexInputStateBuilder&;

    [[nodiscard]] auto build(const Effect& t_effect) const noexcept -> VertexInputState;

private:
    std::vector<Binding> m_bindings;
};

}   // namespace engine::renderer
