#pragma once

#include <span>

#include <vulkan/vulkan.hpp>

namespace core::image {

class Image {
public:
    virtual ~Image() = default;

    [[nodiscard]]
    virtual auto clone() const -> std::unique_ptr<Image> = 0;

    [[nodiscard]]
    virtual auto data() const noexcept -> std::span<const std::byte> = 0;

    [[nodiscard]]
    virtual auto width() const noexcept -> uint32_t = 0;
    [[nodiscard]]
    virtual auto height() const noexcept -> uint32_t = 0;
    [[nodiscard]]
    virtual auto depth() const noexcept -> uint32_t = 0;
    [[nodiscard]]
    auto extent() const noexcept -> vk::Extent3D;

    [[nodiscard]]
    virtual auto mip_level_count() const noexcept -> uint32_t = 0;
    [[nodiscard]]
    virtual auto needs_mip_generation() const noexcept -> bool = 0;

    [[nodiscard]]
    virtual auto format() const noexcept -> vk::Format = 0;

    [[nodiscard]]
    virtual auto
        offset_of(uint32_t mip_level, uint32_t layer, uint32_t face_slice) const noexcept
        -> uint64_t = 0;
};

}   // namespace core::image
