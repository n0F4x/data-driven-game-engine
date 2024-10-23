#pragma once

#include <filesystem>
#include <span>

#include <gsl-lite/gsl-lite.hpp>

#include <stb_image.h>

#include "core/image/Image.hpp"

namespace core::image::stb {

class Image : public image::Image {
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
    auto offset_of(uint32_t mip_level, uint32_t layer, uint32_t face_slice) const noexcept
        -> uint64_t final;

private:
    gsl_lite::not_null<std::unique_ptr<stbi_uc, decltype(&stbi_image_free)>> m_data;
    int                                                                      m_width;
    int                                                                      m_height;

    explicit Image(gsl_lite::not_null<stbi_uc*> data, int width, int height) noexcept;
};

}   // namespace core::image::stb
