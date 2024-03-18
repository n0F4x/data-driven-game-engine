#pragma once

#include <filesystem>

#include <tl/optional.hpp>

#include "core/common/Cache.hpp"
#include "core/common/Handle.hpp"
#include "core/renderer/base/Allocator.hpp"
#include "core/renderer/material_system/VertexInputStateBuilder.hpp"

#include "ImageLoader.hpp"
#include "Model.hpp"
#include "RenderModel.hpp"
#include "StagingModel.hpp"

namespace core::renderer {

class ModelLoader {
public:
    ModelLoader() noexcept = default;
    explicit ModelLoader(Cache& t_cache) noexcept;

    [[nodiscard]] static auto load_from_file(
        const std::filesystem::path& t_filepath,
        const renderer::Allocator&   t_allocator
    ) noexcept -> tl::optional<StagingModel>;

    [[nodiscard]] static auto load_from_file(
        const std::filesystem::path&   t_filepath,
        const VertexInputStateBuilder& t_vertex_input_state
    ) noexcept -> tl::optional<Handle<Model>>;

private:
    tl::optional<Cache&>       m_cache;
    std::vector<Handle<Image>> m_images;
};

}   // namespace core::renderer
