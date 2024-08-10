#include "ImageLoader.hpp"

#include <fmt/format.h>

#include "core/image/KTX2Image.hpp"
#include "core/image/StbImage.hpp"

namespace core::gltf {

auto ImageLoader::load_from_file(const std::filesystem::path& t_filepath
) -> std::optional<Image>
{
    return image::StbImage::load_from_file(t_filepath)
        .transform([](image::StbImage image) -> Image {
            return std::make_unique<image::StbImage>(std::move(image));
        })
        .or_else([&t_filepath] {
            return image::KTX2Image::load_from_file(t_filepath)
                .transform([](image::KTX2Image image) -> Image {
                    return std::make_unique<image::KTX2Image>(std::move(image));
                });
        });
}

auto ImageLoader::load_from_memory(
    const std::span<const std::uint8_t> t_data,
    const fastgltf::MimeType            t_mime_type
) -> std::optional<Image>
{
    switch (t_mime_type) {
        case fastgltf::MimeType::PNG: [[fallthrough]];
        case fastgltf::MimeType::JPEG: {
            return image::StbImage::load_from_memory(t_data).transform(
                [](image::StbImage image) {
                    return std::make_unique<image::StbImage>(std::move(image));
                }
            );
        }
        case fastgltf::MimeType::KTX2: {
            return image::KTX2Image::load_from_memory(t_data).transform(
                [](image::KTX2Image image) {
                    return std::make_unique<image::KTX2Image>(std::move(image));
                }
            );
        }
        default: {
            // TODO: use std::format
            throw std::runtime_error(fmt::format(
                "Unsupported mime type for loading images: {}",
                std::to_underlying(t_mime_type)
            ));
        }
    }
}

}   // namespace core::gltf
