#pragma once

#include <filesystem>
#include <optional>
#include <span>

#include <gsl-lite/gsl-lite.hpp>

#include <ktx.h>

#include "core/image/Image.hpp"

namespace core::image::ktx2 {

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
    auto mip_levels() const noexcept -> uint32_t final;

    [[nodiscard]]
    auto format() const noexcept -> vk::Format final;

    [[nodiscard]]
    auto offset(uint32_t mip_level, uint32_t layer, uint32_t face_slice) const noexcept
        -> uint64_t final;

private:
    struct Deleter {
        auto operator()(ktxTexture2* texture) const noexcept -> void;
    };

    gsl_lite::not_null<std::unique_ptr<ktxTexture2, Deleter>> m_impl;

    explicit Image(gsl_lite::not_null<ktxTexture2*> texture) noexcept;
};

}   // namespace core::image::ktx2
