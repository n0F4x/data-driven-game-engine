#pragma once

#include <filesystem>
#include <functional>
#include <variant>

#include <tl/optional.hpp>

#include <entt/core/fwd.hpp>

#include "engine/renderer/base/Allocator.hpp"
#include "engine/renderer/ResourceManager.hpp"

#include "ImageLoader.hpp"
#include "Model.hpp"
#include "RenderModel.hpp"
#include "StagingModel.hpp"

namespace engine::renderer {

class ModelLoader {
public:
    [[nodiscard]] static auto hash(const std::filesystem::path& t_filepath)
        -> entt::id_type;

    ModelLoader() noexcept = default;
    explicit ModelLoader(ResourceManager& t_resource_manager) noexcept;

    [[nodiscard]] static auto load_from_file(
        const std::filesystem::path& t_filepath,
        const renderer::Allocator&   t_allocator
    ) noexcept -> tl::optional<StagingModel>;

    [[nodiscard]] auto load(const std::filesystem::path& t_filepath) noexcept
        -> tl::optional<Model>;

private:
    tl::optional<ResourceManager&> m_resource_manager{};
};

}   // namespace engine::renderer
