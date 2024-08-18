#include "StbImage.hpp"

#include <spdlog/spdlog.h>

#include <vulkan/vulkan.hpp>

namespace core::image {

auto StbImage::load_from_file(const std::filesystem::path& filepath)
    -> std::optional<StbImage>
{
    if (stbi_info(filepath.generic_string().c_str(), nullptr, nullptr, nullptr) != 1) {
        return std::nullopt;
    }

    int      width{};
    int      height{};
    stbi_uc* data{
        // TODO: request format
        stbi_load(
            filepath.generic_string().c_str(), &width, &height, nullptr, STBI_rgb_alpha
        )
    };

    if (data == nullptr) {
        SPDLOG_ERROR(stbi_failure_reason());
        return std::nullopt;
    }

    return StbImage{ gsl::make_not_null(data), width, height, STBI_rgb_alpha };
}

auto StbImage::load_from_memory(const std::span<const std::byte> data)
    -> std::optional<StbImage>
{
    if (stbi_info_from_memory(
            reinterpret_cast<const stbi_uc*>(data.data()),
            static_cast<int>(data.size()),
            nullptr,
            nullptr,
            nullptr
        )
        != 1)
    {
        return std::nullopt;
    }

    int      width{};
    int      height{};
    // TODO: request format
    stbi_uc* image_data{ stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(data.data()),
        static_cast<int>(data.size()),
        &width,
        &height,
        nullptr,
        STBI_rgb_alpha
    ) };

    if (image_data == nullptr) {
        SPDLOG_ERROR(stbi_failure_reason());
        return std::nullopt;
    }

    return StbImage{ gsl::make_not_null(image_data), width, height, STBI_rgb_alpha };
}

auto StbImage::data() const noexcept -> std::span<const std::byte>
{
    const size_t size{ static_cast<size_t>(m_width) * static_cast<size_t>(m_height)
                       * static_cast<size_t>(m_channel_count) };

    return std::as_bytes(std::span{ m_data.get(), size });
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

auto StbImage::offset(const uint32_t mip_level, const uint32_t, const uint32_t)
    const noexcept -> uint64_t
{
    // TODO more assertions
    assert(mip_level < mip_levels());
    return 0;
}

StbImage::StbImage(
    const gsl_lite::not_null<stbi_uc*>  data,
    const int      width,
    const int      height,
    const int      channel_count
) noexcept
    : m_data{ std::unique_ptr<stbi_uc, decltype(&stbi_image_free)>{ data, stbi_image_free } },
      m_width{ width },
      m_height{ height },
      m_channel_count{ channel_count }
{}

}   // namespace core::image
