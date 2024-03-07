#define STBI_FAILURE_USERMSG
#include "ImageLoader.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

namespace engine::renderer {

namespace tag {

struct STB;

struct KTX;

}   // namespace tag

namespace internal {

template <typename Tag = void>
[[nodiscard]] static auto load_from_file(const std::filesystem::path& t_filepath)
    -> tl::optional<ImageInfo>;

template <>
[[nodiscard]] auto load_from_file<tag::STB>(const std::filesystem::path& t_filepath)
    -> tl::optional<ImageInfo>
{
    int width;
    int height;
    int channels_in_file;

    if (!stbi_info(
            t_filepath.generic_string().c_str(), &width, &height, &channels_in_file
        ))
    {
        return tl::nullopt;
    }

    stbi_uc* data{ stbi_load(
        t_filepath.generic_string().c_str(), &width, &height, &channels_in_file, STBI_rgb_alpha
    ) };

    if (data == nullptr) {
        SPDLOG_ERROR(stbi_failure_reason());
        return tl::nullopt;
    }

    vk::Extent3D extent{ .width  = static_cast<uint32_t>(width),
                         .height = static_cast<uint32_t>(height),
                         .depth  = 1u };
    return ImageInfo{
        .hash = ImageLoader::hash(t_filepath),
        .info = StbImage{ .extent = extent, .data = { data, stbi_image_free } }
    };
}

template <>
[[nodiscard]] auto load_from_file<tag::KTX>(const std::filesystem::path& t_filepath)
    -> tl::optional<ImageInfo>
{
    ktxTexture* ktxTexture;

    if (ktxResult result{ ktxTexture_CreateFromNamedFile(
            t_filepath.generic_string().c_str(),
            KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
            &ktxTexture
        ) };
        result != KTX_SUCCESS && result != KTX_UNKNOWN_FILE_FORMAT)
    {
        SPDLOG_ERROR(
            "ktxTexture_CreateFromNamedFile failed with {}", ktxErrorString(result)
        );

        return tl::nullopt;
    }

    return ImageInfo{
        .hash = ImageLoader::hash(t_filepath),
        .info = KtxImageInfo{ ktxTexture,
                             [](struct ktxTexture* p) { ktxTexture_Destroy(p); } }
    };
}

template <typename Tag = void>
[[nodiscard]] static auto load_from_memory(const std::span<const std::uint8_t> t_data)
    -> tl::optional<ImageInfo>;

template <>
[[nodiscard]] auto load_from_memory<tag::STB>(const std::span<const std::uint8_t>)
    -> tl::optional<ImageInfo>
{
    return {};
}

template <>
[[nodiscard]] auto load_from_memory<tag::KTX>(const std::span<const std::uint8_t>)
    -> tl::optional<ImageInfo>
{
    return {};
}

}   // namespace internal

auto ImageLoader::hash(const std::filesystem::path& t_filepath) -> entt::id_type
{
    std::string absolute_path{ std::filesystem::absolute(t_filepath).generic_string() };
    return entt::hashed_string{ absolute_path.c_str(), absolute_path.size() };
}

ImageLoader::ImageLoader(ResourceManager& t_resource_manager) noexcept
    : m_resource_manager{ t_resource_manager }
{}

auto ImageLoader::load_from_file(const std::filesystem::path& t_filepath)
    -> tl::optional<ImageInfo>
{
    return internal::load_from_file<tag::STB>(t_filepath).or_else([&]() {
        return internal::load_from_file<tag::KTX>(t_filepath);
    });
}

auto ImageLoader::load_from_memory(const std::span<const std::uint8_t> t_data)
    -> tl::optional<ImageInfo>
{
    return internal::load_from_memory<tag::STB>(t_data).or_else([&]() {
        return internal::load_from_memory<tag::KTX>(t_data);
    });
}

}   // namespace engine::renderer
