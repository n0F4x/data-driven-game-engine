#pragma once

#include <filesystem>
#include <optional>
#include <span>

#include <gsl-lite/gsl-lite.hpp>

#include <stb_image.h>

#include "Image.hpp"

namespace core::image {

class StbImage : public Image {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& t_filepath
    ) -> std::optional<StbImage>;

    [[nodiscard]]
    static auto load_from_memory(std::span<const std::uint8_t> t_data
    ) -> std::optional<StbImage>;

    [[nodiscard]]
    auto data() const noexcept -> void* final;
    [[nodiscard]]
    auto size() const noexcept -> size_t final;

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
    gsl_lite::not_null<std::unique_ptr<stbi_uc, decltype(&stbi_image_free)>> m_data;
    int                                                                      m_width;
    int                                                                      m_height;
    int m_channel_count;

    explicit StbImage(
        gsl_lite::not_null<stbi_uc*> data,
        int                          width,
        int                          height,
        int                          channel_count
    ) noexcept;
};

}   // namespace core::image
