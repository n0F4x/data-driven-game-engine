#pragma once

#include <filesystem>
#include <optional>
#include <span>

#include <fastgltf/core.hpp>

#include "core/cache/Cache.hpp"
#include "core/cache/Handle.hpp"

#include "Model.hpp"

namespace core::graphics {

class ImageLoader {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& t_filepath
    ) -> std::optional<Model::Image>;

    [[nodiscard]]
    static auto load_from_memory(
        std::span<const std::uint8_t> t_data,
        fastgltf::MimeType            t_mime_type
    ) -> std::optional<Model::Image>;
};

}   // namespace core::graphics
