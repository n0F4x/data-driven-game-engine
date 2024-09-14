#pragma once

#include <filesystem>
#include <optional>
#include <span>

#include <fastgltf/core.hpp>

#include "Image.hpp"

namespace core::gltf {

class ImageLoader {
public:
    [[nodiscard]]
    static auto load_from(const std::filesystem::path& filepath) -> std::optional<Image>;

    [[nodiscard]]
    static auto load_from(std::span<const std::byte> data, fastgltf::MimeType mime_type)
        -> std::optional<Image>;
};

}   // namespace core::gltf
