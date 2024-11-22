module;

#include <filesystem>
#include <span>

export module core.image.stb.Image;

import vulkan_hpp;

import core.image.Image;

namespace core::image::stb {

export class Image : public image::Image {
public:
    [[nodiscard]]
    static auto load_from(const std::filesystem::path& filepath) -> Image;

    [[nodiscard]]
    static auto load_from(std::span<const std::byte> data) -> Image;

    [[nodiscard]]
    auto data() const noexcept -> std::span<const std::byte> final;

    [[nodiscard]]
    auto width() const noexcept -> uint32_t final;
    [[nodiscard]]
    auto height() const noexcept -> uint32_t final;
    [[nodiscard]]
    auto depth() const noexcept -> uint32_t final;

    [[nodiscard]]
    auto mip_level_count() const noexcept -> uint32_t final;
    [[nodiscard]]
    auto needs_mip_generation() const noexcept -> bool final;

    [[nodiscard]]
    auto format() const noexcept -> vk::Format final;

    [[nodiscard]]
    auto offset_of(
        uint32_t mip_level_index,
        uint32_t layer,
        uint32_t face_slice
    ) const noexcept -> uint64_t final;

private:
    std::vector<std::byte> m_data;
    uint32_t               m_base_width;
    uint32_t               m_base_height;
    vk::Format             m_format;
    uint32_t               m_mip_level_count;

    explicit Image(
        std::vector<std::byte>&& data,
        uint32_t                 base_width,
        uint32_t                 base_height,
        vk::Format               format,
        uint32_t                 mip_level_count
    ) noexcept;
};

}   // namespace core::image::stb
