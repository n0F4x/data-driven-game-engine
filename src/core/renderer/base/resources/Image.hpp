#pragma once

#include <vulkan/vulkan.hpp>

#include "core/utility/lifetime_bound.hpp"

namespace core::renderer::base {

class Allocator;

class Image {
public:
    struct State {
        vk::PipelineStageFlags stage_mask{ vk::PipelineStageFlagBits::eTopOfPipe };
        vk::AccessFlags        access_mask;
        vk::ImageLayout        layout{};
    };

    Image() = default;

    [[nodiscard]]
    auto get() const [[lifetime_bound]] -> vk::Image;

    [[nodiscard]]
    auto format() const -> vk::Format;

    [[nodiscard]]
    auto extent() const -> const vk::Extent3D&;

    [[nodiscard]]
    auto mip_level_count() const -> uint32_t;

    [[nodiscard]]
    auto layout() const -> vk::ImageLayout;

    auto reset() -> void;

    auto transition(const State& new_state) -> State;

private:
    friend Allocator;

    vk::UniqueImage m_image;

    vk::Format   m_format{};
    vk::Extent3D m_extent{};
    uint32_t     m_mip_level_count{};

    State m_state;

    Image(vk::UniqueImage&& image, const vk::ImageCreateInfo& create_info);
};

}   // namespace core::renderer::base
