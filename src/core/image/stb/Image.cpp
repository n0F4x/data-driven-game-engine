#include "Image.hpp"

#include <ranges>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_format_traits.hpp>

#include <stb_image.h>
#include <stb_image_resize.h>

[[nodiscard]]
static auto count_mip_levels(const uint32_t base_width, const uint32_t base_height)
    -> uint32_t
{
    return static_cast<uint32_t>(std::floor(std::log2(std::max(base_width, base_height))))
         + 1;
}

[[nodiscard]]
static auto mip_level_size(
    const uint32_t width,
    const uint32_t height,
    const uint32_t component_count
) -> size_t
{
    return static_cast<size_t>(width) * static_cast<size_t>(height)
         * static_cast<size_t>(component_count);
}

[[nodiscard]]
static auto mipped_image_size(
    const uint32_t base_width,
    const uint32_t base_height,
    const uint32_t component_count,
    const uint32_t mip_level_count
) -> size_t
{
    size_t result{};

    // TODO: use std::views::cartesian_product
    uint32_t width{ base_width };
    uint32_t height{ base_height };
    uint32_t mip_level_index{};
    while (mip_level_index < mip_level_count) {
        result += ::mip_level_size(width, height, component_count);

        width  = std::max(width / 2u, 1u);
        height = std::max(height / 2u, 1u);
        ++mip_level_index;
    }

    return result;
}

[[nodiscard]]
static auto generate_mip_maps(
    const std::span<const std::byte> base_image_data,
    const uint32_t                   base_width,
    const uint32_t                   base_height,
    const uint32_t                   component_count,
    const uint32_t                   alpha_index,
    const uint32_t                   mip_level_count
) -> std::vector<std::byte>
{
    std::vector<std::byte> result;
    result.resize(
        ::mipped_image_size(base_width, base_height, component_count, mip_level_count)
    );

    std::memcpy(result.data(), base_image_data.data(), base_image_data.size_bytes());

    uint32_t width{ base_width };
    uint32_t height{ base_height };
    size_t   offset{};
    for (const auto _ : std::views::iota(0u, mip_level_count)) {
        const uint32_t next_width{ std::max(width / 2u, 1u) };
        const uint32_t next_height{ std::max(height / 2u, 1u) };
        const size_t   next_offset{ offset
                                  + ::mip_level_size(width, height, component_count) };

        ::stbir_resize_uint8_srgb(
            reinterpret_cast<const unsigned char*>(&result[offset]),
            static_cast<int>(width),
            static_cast<int>(height),
            0,
            reinterpret_cast<unsigned char*>(&result[next_offset]),
            static_cast<int>(next_width),
            static_cast<int>(next_height),
            0,
            static_cast<int>(component_count),
            static_cast<int>(alpha_index),
            0
        );

        width  = next_width;
        height = next_height;
        offset = next_offset;
    }

    return result;
}

auto core::image::stb::Image::load_from(const std::filesystem::path& filepath) -> Image
{
    // TODO: request format
    constexpr static uint8_t    alpha_index{ 3 };
    constexpr static vk::Format format{ vk::Format::eR8G8B8A8Srgb };
    constexpr static uint8_t    component_count{ vk::componentCount(format) };

    int      width{};
    int      height{};
    stbi_uc* raw_image_data{ ::stbi_load(
        filepath.generic_string().c_str(), &width, &height, nullptr, component_count
    ) };

    if (raw_image_data == nullptr) {
        throw std::runtime_error{
            std::format("`stbi_load` failed: ", stbi_failure_reason())
        };
    }

    const uint32_t mip_level_count{
        ::count_mip_levels(static_cast<uint32_t>(width), static_cast<uint32_t>(height))
    };

    std::vector<std::byte> image_data{ ::generate_mip_maps(
        std::as_bytes(std::span{
            raw_image_data,
            ::mip_level_size(
                static_cast<uint32_t>(width), static_cast<uint32_t>(height), component_count
            ) }),
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        component_count,
        alpha_index,
        mip_level_count
    ) };

    ::stbi_image_free(raw_image_data);

    return Image{ std::move(image_data),
                  static_cast<uint32_t>(width),
                  static_cast<uint32_t>(height),
                  format,
                  mip_level_count };
}

auto core::image::stb::Image::load_from(const std::span<const std::byte> data) -> Image
{
    // TODO: request format
    constexpr static uint8_t    alpha_index{ 3 };
    constexpr static vk::Format format{ vk::Format::eR8G8B8A8Srgb };
    constexpr static uint8_t    component_count{ vk::componentCount(format) };

    int      width{};
    int      height{};
    stbi_uc* raw_image_data{ ::stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(data.data()),
        static_cast<int>(data.size()),
        &width,
        &height,
        nullptr,
        component_count
    ) };

    if (raw_image_data == nullptr) {
        throw std::runtime_error{ ::stbi_failure_reason() };
    }

    const uint32_t mip_level_count{
        ::count_mip_levels(static_cast<uint32_t>(width), static_cast<uint32_t>(height))
    };

    std::vector<std::byte> image_data{ ::generate_mip_maps(
        std::as_bytes(std::span{
            raw_image_data,
            ::mip_level_size(
                static_cast<uint32_t>(width), static_cast<uint32_t>(height), component_count
            ) }),
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        component_count,
        alpha_index,
        mip_level_count
    ) };

    ::stbi_image_free(raw_image_data);

    return Image{ std::move(image_data),
                  static_cast<uint32_t>(width),
                  static_cast<uint32_t>(height),
                  format,
                  mip_level_count };
}

auto core::image::stb::Image::clone() const -> std::unique_ptr<image::Image>
{
    return std::make_unique<Image>(*this);
}

auto core::image::stb::Image::data() const noexcept -> std::span<const std::byte>
{
    return m_data;
}

auto core::image::stb::Image::width() const noexcept -> uint32_t
{
    return m_base_width;
}

auto core::image::stb::Image::height() const noexcept -> uint32_t
{
    return m_base_height;
}

auto core::image::stb::Image::depth() const noexcept -> uint32_t
{
    return 1;
}

auto core::image::stb::Image::mip_level_count() const noexcept -> uint32_t
{
    return m_mip_level_count;
}

auto core::image::stb::Image::needs_mip_generation() const noexcept -> bool
{
    return false;
}

auto core::image::stb::Image::format() const noexcept -> vk::Format
{
    return m_format;
}

auto core::image::stb::Image::offset_of(
    const uint32_t mip_level_index,
    const uint32_t,
    const uint32_t
) const noexcept -> uint64_t
{
    uint64_t result{};

    uint32_t width{ m_base_width };
    uint32_t height{ m_base_height };
    for (const auto _ : std::views::iota(0u, mip_level_index)) {
        const uint32_t next_width{ std::max(width / 2u, 1u) };
        const uint32_t next_height{ std::max(height / 2u, 1u) };

        result += ::mip_level_size(width, height, vk::componentCount(m_format));

        width  = next_width;
        height = next_height;
    }

    return result;
}

core::image::stb::Image::Image(
    std::vector<std::byte>&& data,
    const uint32_t           base_width,
    const uint32_t           base_height,
    const vk::Format         format,
    const uint32_t           mip_level_count
) noexcept
    : m_data{ std::move(data) },
      m_base_width{ base_width },
      m_base_height{ base_height },
      m_format{ format },
      m_mip_level_count{ mip_level_count }
{}
