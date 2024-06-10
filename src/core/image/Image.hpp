#pragma once

namespace vk {

enum class Format;

}   // namespace vk

namespace core::image {

class Image {
public:
    virtual ~Image() = default;

    [[nodiscard]]
    virtual auto data() const noexcept -> void* = 0;
    [[nodiscard]]
    virtual auto size() const noexcept -> size_t = 0;

    [[nodiscard]]
    virtual auto width() const noexcept -> uint32_t = 0;
    [[nodiscard]]
    virtual auto height() const noexcept -> uint32_t = 0;
    [[nodiscard]]
    virtual auto depth() const noexcept -> uint32_t = 0;

    [[nodiscard]]
    virtual auto mip_levels() const noexcept -> uint32_t = 0;

    [[nodiscard]]
    virtual auto format() const noexcept -> vk::Format = 0;

    [[nodiscard]]
    virtual auto offset(uint32_t mip_level, uint32_t layer, uint32_t face_slice)
        const noexcept -> uint64_t = 0;
};

}   // namespace core::image
