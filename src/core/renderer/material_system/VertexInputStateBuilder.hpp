#pragma once

#include <tl/optional.hpp>

#include "Effect.hpp"
#include "VertexAttribute.hpp"

namespace core::renderer {

class VertexInputState;

class VertexInputStateBuilder {
public:
    struct AttributeInfo {
        vk::Format format;
        uint32_t   offset;
    };

    class Binding {
    public:
        class Attributes {
        public:
            [[nodiscard]] auto operator[](VertexAttribute t_attributes) noexcept
                -> tl::optional<AttributeInfo>&;
            [[nodiscard]] auto operator[](VertexAttribute t_attributes) const noexcept
                -> const tl::optional<AttributeInfo>&;

        private:
            std::array<
                tl::optional<AttributeInfo>,
                static_cast<size_t>(VertexAttribute::COUNT)>
                m_attributes;
        };

        explicit Binding(uint32_t t_stride) noexcept;

        [[nodiscard]] auto stride() const noexcept -> uint32_t;
        [[nodiscard]] auto attributes() const noexcept -> const Attributes&;

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

}   // namespace core::renderer
