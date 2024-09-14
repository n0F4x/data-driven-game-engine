#include "ImageLoader.hpp"

#include <algorithm>
#include <ranges>

#include <spdlog/spdlog.h>

#include "core/image/jpeg/MimeType.hpp"
#include "core/image/ktx2/Image.hpp"
#include "core/image/ktx2/MimeType.hpp"
#include "core/image/png/MimeType.hpp"
#include "core/image/stb/Image.hpp"

using namespace core;

template <
    size_t N = std::max({ image::png::MimeType::magic().size_bytes(),
                          image::jpeg::MimeType::magic().size_bytes(),
                          image::ktx2::MimeType::magic().size_bytes() })>
[[nodiscard]]
static auto read_n_from(const std::filesystem::path& filepath)
    -> std::array<std::ifstream::char_type, N>
{
    std::ifstream file{ filepath, std::ios::binary | std::ios::in };

    std::array<std::ifstream::char_type, N> first_characters{};

    file.seekg(0, std::ios::beg);
    file.read(first_characters.data(), first_characters.size());

    return first_characters;
}

namespace {

enum class SupportedMimeTypes {
    ePNG,
    eJPG,
    eKTX2
};

}   // namespace

[[nodiscard]]
static auto mime_type(std::ranges::range auto&& first_characters)
    -> std::optional<SupportedMimeTypes>
{
    const auto first_bytes{ first_characters
                            | std::views::transform([](const std::ifstream::char_type byte) {
                                  return static_cast<std::byte>(byte);
                              }) };

    using enum SupportedMimeTypes;
    if (std::ranges::starts_with(first_bytes, image::png::MimeType::magic())) {
        return ePNG;
    }
    if (std::ranges::starts_with(first_bytes, image::jpeg::MimeType::magic())) {
        return eJPG;
    }
    if (std::ranges::starts_with(first_bytes, image::ktx2::MimeType::magic())) {
        return eKTX2;
    }

    return std::nullopt;
}

auto core::gltf::ImageLoader::load_from(const std::filesystem::path& filepath)
    -> std::optional<Image>
{
    return ::mime_type(read_n_from(filepath))
        .transform([&filepath](const SupportedMimeTypes mime_type) -> Image {
            using enum SupportedMimeTypes;
            switch (mime_type) {
                case ePNG:
                case eJPG:
                    return std::make_unique<image::stb::Image>(
                        image::stb::Image::load_from(filepath)
                    );
                case eKTX2:
                    return std::make_unique<image::ktx2::Image>(
                        image::ktx2::Image::load_from(filepath)
                    );
            }
            std::unreachable();
        });
}

auto core::gltf::ImageLoader::load_from(
    const std::span<const std::byte> data,
    const fastgltf::MimeType         mime_type
) -> std::optional<Image>
{
    switch (mime_type) {
        case fastgltf::MimeType::PNG:
        case fastgltf::MimeType::JPEG:
            return std::make_unique<image::stb::Image>(image::stb::Image::load_from(data));
        case fastgltf::MimeType::KTX2:
            return std::make_unique<image::ktx2::Image>(image::ktx2::Image::load_from(data));
        default: {
            SPDLOG_WARN(
                "Unsupported mime type for loading images: {}", std::to_underlying(mime_type)
            );
            return std::nullopt;
        }
    }
}
