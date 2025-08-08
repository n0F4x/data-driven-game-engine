module;

#include <cmath>
#include <filesystem>
#include <format>
#include <ranges>

#include <vulkan/vulkan_format_traits.hpp>

#include <stb_image.h>
#include <stb_image_resize2.h>

module modules.image.png.Image;

[[nodiscard]]
static auto count_mip_levels(const uint32_t base_width, const uint32_t base_height)
    -> uint32_t
{
    return static_cast<uint32_t>(std::floor(std::log2(std::max(base_width, base_height))))
         + 1;
}

[[nodiscard]]
static auto
    mip_level_size(const uint32_t width, const uint32_t height, const uint32_t block_size)
        -> std::size_t
{
    return static_cast<std::size_t>(width) * static_cast<std::size_t>(height)
         * static_cast<std::size_t>(block_size);
}

[[nodiscard]]
static auto mipped_image_size(
    const uint32_t base_width,
    const uint32_t base_height,
    const uint32_t block_size,
    const uint32_t mip_level_count
) -> std::size_t
{
    std::size_t result{};

    // TODO: use std::views::cartesian_product
    uint32_t width{ base_width };
    uint32_t height{ base_height };
    uint32_t mip_level_index{};
    while (mip_level_index < mip_level_count) {
        result += ::mip_level_size(width, height, block_size);

        width  = std::max(width / 2u, 1u);
        height = std::max(height / 2u, 1u);
        ++mip_level_index;
    }

    return result;
}

[[nodiscard]]
static auto pixel_layout_from([[maybe_unused]] const vk::Format format)
    -> stbir_pixel_layout
{
    assert(format == vk::Format::eR8G8B8A8Srgb);
    return stbir_pixel_layout::STBIR_RGBA;
}

[[nodiscard]]
static auto generate_mip_maps(
    const std::span<const std::byte> base_image_data,
    const uint32_t                   base_width,
    const uint32_t                   base_height,
    const vk::Format                 format,
    const uint32_t                   mip_level_count
) -> std::vector<std::byte>
{
    std::vector<std::byte> result;
    result.resize(
        ::mipped_image_size(base_width, base_height, vk::blockSize(format), mip_level_count)
    );

    std::memcpy(result.data(), base_image_data.data(), base_image_data.size_bytes());

    uint32_t    width{ base_width };
    uint32_t    height{ base_height };
    std::size_t offset{};
    for (const auto _ : std::views::iota(0u, mip_level_count)) {
        const uint32_t    next_width{ std::max(width / 2u, 1u) };
        const uint32_t    next_height{ std::max(height / 2u, 1u) };
        const std::size_t next_offset{
            offset + ::mip_level_size(width, height, vk::blockSize(format))
        };

        ::stbir_resize_uint8_srgb(
            reinterpret_cast<const unsigned char*>(&result[offset]),
            static_cast<int>(width),
            static_cast<int>(height),
            0,
            reinterpret_cast<unsigned char*>(&result[next_offset]),
            static_cast<int>(next_width),
            static_cast<int>(next_height),
            0,
            ::pixel_layout_from(format)
        );

        width  = next_width;
        height = next_height;
        offset = next_offset;
    }

    return result;
}

auto modules::image::png::Image::load_from(const std::filesystem::path& filepath) -> Image
{
    // TODO: request format
    constexpr static vk::Format format{ vk::Format::eR8G8B8A8Srgb };

    int      width{};
    int      height{};
    stbi_uc* raw_image_data{ ::stbi_load(
        filepath.generic_string().c_str(),
        &width,
        &height,
        nullptr,
        vk::componentCount(format)
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
        std::as_bytes(
            std::span{ raw_image_data,
                       ::mip_level_size(
                           static_cast<uint32_t>(width),
                           static_cast<uint32_t>(height),
                           vk::blockSize(format)
                       ) }
        ),
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        format,
        mip_level_count
    ) };

    ::stbi_image_free(raw_image_data);

    return Image{ std::move(image_data),
                  static_cast<uint32_t>(width),
                  static_cast<uint32_t>(height),
                  format,
                  mip_level_count };
}

auto modules::image::png::Image::load_from(const std::span<const std::byte> data) -> Image
{
    // TODO: request format
    constexpr static vk::Format format{ vk::Format::eR8G8B8A8Srgb };

    int      width{};
    int      height{};
    stbi_uc* raw_image_data{ ::stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(data.data()),
        static_cast<int>(data.size()),
        &width,
        &height,
        nullptr,
        vk::componentCount(format)
    ) };

    if (raw_image_data == nullptr) {
        throw std::runtime_error{ ::stbi_failure_reason() };
    }

    const uint32_t mip_level_count{
        ::count_mip_levels(static_cast<uint32_t>(width), static_cast<uint32_t>(height))
    };

    std::vector<std::byte> image_data{ ::generate_mip_maps(
        std::as_bytes(
            std::span{ raw_image_data,
                       ::mip_level_size(
                           static_cast<uint32_t>(width),
                           static_cast<uint32_t>(height),
                           vk::blockSize(format)
                       ) }
        ),
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        format,
        mip_level_count
    ) };

    ::stbi_image_free(raw_image_data);

    return Image{ std::move(image_data),
                  static_cast<uint32_t>(width),
                  static_cast<uint32_t>(height),
                  format,
                  mip_level_count };
}

auto modules::image::png::Image::data() const noexcept -> std::span<const std::byte>
{
    return m_data;
}

auto modules::image::png::Image::width() const noexcept -> uint32_t
{
    return m_base_width;
}

auto modules::image::png::Image::height() const noexcept -> uint32_t
{
    return m_base_height;
}

auto modules::image::png::Image::depth() const noexcept -> uint32_t
{
    return 1;
}

auto modules::image::png::Image::mip_level_count() const noexcept -> uint32_t
{
    return m_mip_level_count;
}

auto modules::image::png::Image::needs_mip_generation() const noexcept -> bool
{
    return false;
}

auto modules::image::png::Image::format() const noexcept -> vk::Format
{
    return m_format;
}

auto modules::image::png::Image::offset_of(
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

        result += ::mip_level_size(width, height, vk::blockSize(m_format));

        width  = next_width;
        height = next_height;
    }

    return result;
}

modules::image::png::Image::Image(
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
