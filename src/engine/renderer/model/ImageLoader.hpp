#pragma once

#include <filesystem>
#include <span>

#include <tl/optional.hpp>

#include <entt/core/fwd.hpp>

#include "engine/renderer/ResourceManager.hpp"

#include "ImageInfo.hpp"

namespace engine::renderer {

class ImageLoader {
public:
    [[nodiscard]] static auto hash(const std::filesystem::path& t_filepath)
        -> entt::id_type;

    ImageLoader() noexcept = default;
    explicit ImageLoader(ResourceManager& t_resource_manager) noexcept;

    [[nodiscard]] static auto load_from_file(const std::filesystem::path& t_filepath)
        -> tl::optional<ImageInfo>;

    [[nodiscard]] static auto load_from_memory(std::span<const std::uint8_t> t_data)
        -> tl::optional<ImageInfo>;

private:
    tl::optional<ResourceManager&> m_resource_manager;
};

}   // namespace engine::renderer
