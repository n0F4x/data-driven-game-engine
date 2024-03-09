#include "ImageLoader.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

namespace engine::renderer {

auto ImageLoader::hash(const std::filesystem::path& t_filepath) -> entt::id_type
{
    std::string absolute_path{ std::filesystem::absolute(t_filepath).generic_string() };
    return entt::hashed_string{ absolute_path.c_str(), absolute_path.size() };
}

auto ImageLoader::load_from_file(const std::filesystem::path& t_filepath)
    -> tl::optional<Image>
{
    return asset::StbImage::load_from_file(t_filepath)
        .transform([](asset::StbImage&& image) -> Image { return image; })
        .or_else([&]() { return asset::KtxImage::load_from_file(t_filepath); });
}

auto ImageLoader::load_from_memory(const std::span<const std::uint8_t> t_data)
    -> tl::optional<Image>
{
    return asset::StbImage::load_from_memory(t_data)
        .transform([](asset::StbImage&& image) -> Image { return image; })
        .or_else([&]() { return asset::KtxImage::load_from_memory(t_data); });
}

}   // namespace engine::renderer
