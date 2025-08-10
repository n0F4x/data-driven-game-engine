module;

#include <filesystem>
#include <optional>
#include <span>

#include <fastgltf/core.hpp>

export module ddge.modules.gltf.ImageLoader;

import ddge.modules.gltf.Image;

namespace ddge::gltf {

export class ImageLoader {
public:
    [[nodiscard]]
    static auto load_from(const std::filesystem::path& filepath) -> std::optional<Image>;

    [[nodiscard]]
    static auto load_from(std::span<const std::byte> data, fastgltf::MimeType mime_type)
        -> std::optional<Image>;
};

}   // namespace ddge::gltf
