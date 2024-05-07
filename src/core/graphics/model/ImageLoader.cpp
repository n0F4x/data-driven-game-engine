#include "ImageLoader.hpp"

#include "core/asset/image/KtxImage.hpp"
#include "core/asset/image/StbImage.hpp"

namespace core::graphics {

auto ImageLoader::load_from_file(const std::filesystem::path& t_filepath
) -> std::optional<Model::Image>
{
    return asset::StbImage::load_from_file(t_filepath)
        .transform([](asset::StbImage image) {
            return std::make_unique<asset::Image>(std::move(image));
        })
        .or_else([&t_filepath] {
            return asset::KtxImage::load_from_file(t_filepath)
                .transform([](asset::KtxImage image) {
                    return std::make_unique<asset::Image>(std::move(image));
                });
        });
}

auto ImageLoader::load_from_memory(
    const std::span<const std::uint8_t> t_data,
    fastgltf::MimeType                  t_mime_type
) -> std::optional<Model::Image>
{
    switch (t_mime_type) {
        case fastgltf::MimeType::PNG: [[fallthrough]];
        case fastgltf::MimeType::JPEG: {
            return asset::StbImage::load_from_memory(t_data).transform([](auto image) {
                return std::make_unique<asset::Image>(std::forward<decltype(image)>(image)
                );
            });
        }
        case fastgltf::MimeType::KTX2: {
            return asset::KtxImage::load_from_memory(t_data).transform([](auto image) {
                return std::make_unique<asset::Image>(std::forward<decltype(image)>(image)
                );
            });
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
