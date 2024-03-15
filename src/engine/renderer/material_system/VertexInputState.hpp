#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "Effect.hpp"

namespace engine::renderer {

class VertexInputStateBuilder;

class VertexInputState {
public:
    explicit VertexInputState(
        const Effect& t_effect,
        const VertexInputStateBuilder& t_builder
    ) noexcept;

    [[nodiscard]] auto bindings() const noexcept
        -> const std::vector<vk::VertexInputBindingDescription>&;
    [[nodiscard]] auto attributes() const noexcept
        -> const std::vector<vk::VertexInputAttributeDescription>&;
    [[nodiscard]] auto info() const noexcept
        -> const vk::PipelineVertexInputStateCreateInfo&;

private:
    std::vector<vk::VertexInputBindingDescription>   m_bindings;
    std::vector<vk::VertexInputAttributeDescription> m_attributes;
    vk::PipelineVertexInputStateCreateInfo           m_state;

    friend auto hash_value(const VertexInputState& t_vertex_input_layout) noexcept
        -> size_t;
};

}   // namespace engine::renderer

namespace std {

template <>
class hash<engine::renderer::VertexInputState> {
public:
    [[nodiscard]] auto
        operator()(const engine::renderer::VertexInputState& t_vertex_input_layout) const
        -> size_t;
};

}   // namespace std
