#include "StbImage.hpp"

namespace core::asset {

auto StbImage::load_from_file(const std::filesystem::path& t_filepath
) -> std::optional<StbImage>
{
    int width{};
    int height{};
    int channels_in_file{};

    if (stbi_info(t_filepath.generic_string().c_str(), &width, &height, &channels_in_file)
        != 1)
    {
        return std::nullopt;
    }

    stbi_uc* data{ stbi_load(
        t_filepath.generic_string().c_str(), &width, &height, &channels_in_file, STBI_rgb_alpha
    ) };

    if (data == nullptr) {
        SPDLOG_ERROR(stbi_failure_reason());
        return std::nullopt;
    }

    return StbImage{ data, width, height };
}

auto StbImage::load_from_memory(std::span<const std::uint8_t> t_data
) -> std::optional<StbImage>
{
    int width{};
    int height{};
    int channels_in_file{};

    if (stbi_info_from_memory(
            t_data.data(), static_cast<int>(t_data.size()), &width, &height, &channels_in_file
        )
        != 1)
    {
        return std::nullopt;
    }

    stbi_uc* data{ stbi_load_from_memory(
        t_data.data(),
        static_cast<int>(t_data.size()),
        &width,
        &height,
        &channels_in_file,
        STBI_rgb_alpha
    ) };

    if (data == nullptr) {
        SPDLOG_ERROR(stbi_failure_reason());
        return std::nullopt;
    }

    return StbImage{ data, width, height };
}

auto StbImage::data() const noexcept -> void*
{
    return m_data.get();
}

auto StbImage::size() const noexcept -> size_t
{
    return static_cast<size_t>(m_width * m_height * 4);
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
    return 1u;
}

auto StbImage::mip_levels() const noexcept -> uint32_t
{
    return 1u;
}

auto StbImage::format() const noexcept -> vk::Format
{
    return vk::Format::eR8G8B8A8Srgb;
}

StbImage::StbImage(stbi_uc* t_data, int t_width, int t_height) noexcept
    : m_data{ t_data, stbi_image_free },
      m_width{ t_width },
      m_height{ t_height }
{}

}   // namespace core::asset
