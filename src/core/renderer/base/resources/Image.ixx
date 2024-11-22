module;

#include <cstdint>

#include "core/utility/lifetime_bound.hpp"

export module core.renderer.base.resources.Image;

import vulkan_hpp;

namespace core::renderer::base {

export class Image {
public:
    struct State {
        vk::PipelineStageFlags stage_mask{ vk::PipelineStageFlagBits::eTopOfPipe };
        vk::AccessFlags        access_mask;
        vk::ImageLayout        layout{};
    };

    Image() = default;
    Image(vk::UniqueImage&& image, const vk::ImageCreateInfo& create_info);

    [[nodiscard]]
    auto get() const noexcept [[lifetime_bound]] -> vk::Image;

    [[nodiscard]]
    auto device() const noexcept -> vk::Device;

    [[nodiscard]]
    auto format() const noexcept -> vk::Format;

    [[nodiscard]]
    auto aspect_flags() const noexcept -> vk::ImageAspectFlags;

    [[nodiscard]]
    auto extent() const noexcept -> const vk::Extent3D&;

    [[nodiscard]]
    auto mip_level_count() const noexcept -> uint32_t;

    [[nodiscard]]
    auto layout() const noexcept -> vk::ImageLayout;

    auto reset() -> void;

    auto transition(const State& new_state) -> State;

private:
    vk::UniqueImage m_image;

    vk::Format   m_format{};
    vk::Extent3D m_extent{};
    uint32_t     m_mip_level_count{};

    State m_state;
};

}   // namespace core::renderer::base
