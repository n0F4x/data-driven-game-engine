#include "StbImage.hpp"

#include <spdlog/spdlog.h>

#include <vulkan/vulkan.hpp>

namespace core::image {

auto StbImage::load_from_file(const std::filesystem::path& t_filepath
) -> std::optional<StbImage>
{
    if (stbi_info(t_filepath.generic_string().c_str(), nullptr, nullptr, nullptr) != 1) {
        return std::nullopt;
    }

    int      width{};
    int      height{};
    stbi_uc* data{
        // TODO: request format
        stbi_load(
            t_filepath.generic_string().c_str(), &width, &height, nullptr, STBI_rgb_alpha
        )
    };

    if (data == nullptr) {
        SPDLOG_ERROR(stbi_failure_reason());
        return std::nullopt;
    }

    return StbImage{ data, width, height, STBI_rgb_alpha };
}

auto StbImage::load_from_memory(std::span<const std::uint8_t> t_data
) -> std::optional<StbImage>
{
    if (stbi_info_from_memory(
            t_data.data(), static_cast<int>(t_data.size()), nullptr, nullptr, nullptr
        )
        != 1)
    {
        return std::nullopt;
    }

    int      width{};
    int      height{};
    // TODO: request format
    stbi_uc* data{ stbi_load_from_memory(
        t_data.data(), static_cast<int>(t_data.size()), &width, &height, nullptr, STBI_rgb_alpha
    ) };

    if (data == nullptr) {
        SPDLOG_ERROR(stbi_failure_reason());
        return std::nullopt;
    }

    return StbImage{ data, width, height, STBI_rgb_alpha };
}

auto StbImage::data() const noexcept -> void*
{
    return m_data.get();
}

auto StbImage::size() const noexcept -> size_t
{
    return static_cast<size_t>(m_width) * static_cast<size_t>(m_height)
         * static_cast<size_t>(m_channel_count);
}

auto StbImage::width() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(m_width);
}

auto StbImage::height() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(m_height);
}

auto StbImage::depth() const noexcept -> uint32_t
{
    return 1;
}

auto StbImage::mip_levels() const noexcept -> uint32_t
{
    return 1;
}

auto StbImage::format() const noexcept -> vk::Format
{
    switch (m_channel_count) {
        case 1: return vk::Format::eR8Srgb;
        case 2: return vk::Format::eR8G8Srgb;
        case 3: return vk::Format::eR8G8B8Srgb;
        case 4: return vk::Format::eR8G8B8A8Srgb;
        default: std::unreachable();
    };
}

auto StbImage::offset(const uint32_t t_mip_level, const uint32_t, const uint32_t)
    const noexcept -> uint64_t
{
    // TODO more assertions
    assert(t_mip_level < mip_levels());
    return 0;
}

StbImage::StbImage(
    stbi_uc* const t_data,
    const int      t_width,
    const int      t_height,
    const int      t_channel_count
) noexcept
    : m_data{ t_data, stbi_image_free },
      m_width{ t_width },
      m_height{ t_height },
      m_channel_count{ t_channel_count }
{}

}   // namespace core::image