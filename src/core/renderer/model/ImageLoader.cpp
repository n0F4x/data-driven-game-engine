#include "ImageLoader.hpp"

namespace core::renderer {

ImageLoader::ImageLoader(Cache& t_cache) noexcept : m_cache{ t_cache } {}

auto ImageLoader::load_from_file(const std::filesystem::path& t_filepath)
    -> tl::optional<Handle<Image>>
{
    return m_cache
        .and_then([&](const Cache& cache) {
            return cache.find<Image>(std::filesystem::hash_value(t_filepath));
        })
        .or_else([&] {
            return asset::StbImage::load_from_file(t_filepath)
                .transform([](asset::StbImage&& image) {
                    return make_handle<Image>(std::move(image));
                });
        })
        .or_else([&]() {
            return asset::KtxImage::load_from_file(t_filepath)
                .transform([](asset::KtxImage&& image) {
                    return make_handle<Image>(std::move(image));
                });
        });
}

auto ImageLoader::load_from_memory(const std::span<const std::uint8_t> t_data)
    -> tl::optional<Handle<Image>>
{
    return asset::StbImage::load_from_memory(t_data)
        .transform([](asset::StbImage&& image) {
            return make_handle<Image>(std::move(image));
        })
        .or_else([&] {
            return asset::KtxImage::load_from_memory(t_data).transform(
                [](asset::KtxImage&& image) {
                    return make_handle<Image>(std::move(image));
                }
            );
        });
}

}   // namespace core::renderer
