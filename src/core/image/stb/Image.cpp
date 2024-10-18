#include "Image.hpp"

#include <spdlog/spdlog.h>

#include <vulkan/vulkan.hpp>

auto core::image::stb::Image::load_from(const std::filesystem::path& filepath) -> Image
{
    int      width{};
    int      height{};
    stbi_uc* data{
        // TODO: request format
        ::stbi_load(
            filepath.generic_string().c_str(), &width, &height, nullptr, STBI_rgb_alpha
        )
    };

    if (data == nullptr) {
        throw std::runtime_error{
            std::format("`stbi_load` failed: ", stbi_failure_reason())
        };
    }

    return Image{ gsl::make_not_null(data), width, height };
}

auto core::image::stb::Image::load_from(const std::span<const std::byte> data) -> Image
{
    int      width{};
    int      height{};
    // TODO: request format
    stbi_uc* image_data{ ::stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(data.data()),
        static_cast<int>(data.size()),
        &width,
        &height,
        nullptr,
        STBI_rgb_alpha
    ) };

    if (image_data == nullptr) {
        throw std::runtime_error{ ::stbi_failure_reason() };
    }

    return Image{ gsl::make_not_null(image_data), width, height };
}

auto core::image::stb::Image::data() const noexcept -> std::span<const std::byte>
{
    const size_t size{ static_cast<size_t>(m_width) * static_cast<size_t>(m_height)
                       * static_cast<size_t>(STBI_rgb_alpha) };

    return std::as_bytes(std::span{ m_data.get(), size });
}

auto core::image::stb::Image::width() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(m_width);
}

auto core::image::stb::Image::height() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(m_height);
}

auto core::image::stb::Image::depth() const noexcept -> uint32_t
{
    return 1;
}

auto core::image::stb::Image::mip_level_count() const noexcept -> uint32_t
{
    return 0;
}

auto core::image::stb::Image::format() const noexcept -> vk::Format
{
    return vk::Format::eR8G8B8A8Srgb;
}

auto core::image::stb::Image::offset_of(const uint32_t, const uint32_t, const uint32_t)
    const noexcept -> uint64_t
{
    return 0;
}

core::image::stb::Image::Image(
    const gsl_lite::not_null<stbi_uc*>  data,
    const int                           width,
    const int                           height
) noexcept
    : m_data{ std::unique_ptr<stbi_uc, decltype(&::stbi_image_free)>{ data, ::stbi_image_free } },
      m_width{ width },
      m_height{ height }
{}
