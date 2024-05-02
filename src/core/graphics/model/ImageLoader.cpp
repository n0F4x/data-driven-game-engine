#include "ImageLoader.hpp"

namespace core::graphics {

auto ImageLoader::load_from_file(const std::filesystem::path& t_filepath
) -> std::optional<Image>
{
    return asset::StbImage::load_from_file(t_filepath)
        .transform([](asset::StbImage image) { return Image{ std::move(image) }; })
        .or_else([&t_filepath] {
            return asset::KtxImage::load_from_file(t_filepath)
                .transform([](asset::KtxImage image) { return Image{ std::move(image) }; }
                );
        });
}

auto ImageLoader::load_from_memory(
    const std::span<const std::uint8_t> t_data,
    fastgltf::MimeType                  t_mime_type
) -> std::optional<Image>
{
    switch (t_mime_type) {
        case fastgltf::MimeType::PNG: [[fallthrough]];
        case fastgltf::MimeType::JPEG: {
            return asset::StbImage::load_from_memory(t_data);
        }
        case fastgltf::MimeType::KTX2: {
            return asset::KtxImage::load_from_memory(t_data);
        }
        default: {
            throw std::runtime_error(fmt::format(
                "Unsupported mime type for loading images: {}",
                fastgltf::to_underlying(t_mime_type)
            ));
        }
    }
}

}   // namespace core::graphics
