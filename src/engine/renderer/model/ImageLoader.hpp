#pragma once

#include <filesystem>
#include <span>
#include <variant>

#include <tl/optional.hpp>

#include <entt/core/fwd.hpp>

#include "engine/asset/image/KtxImage.hpp"
#include "engine/asset/image/StbImage.hpp"

namespace engine::renderer {

using Image = std::variant<asset::StbImage, asset::KtxImage>;

class ImageLoader {
public:
    [[nodiscard]] static auto hash(const std::filesystem::path& t_filepath)
        -> entt::id_type;

    [[nodiscard]] static auto load_from_file(const std::filesystem::path& t_filepath)
        -> tl::optional<Image>;

    [[nodiscard]] static auto load_from_memory(std::span<const std::uint8_t> t_data)
        -> tl::optional<Image>;
};

}   // namespace engine::renderer
