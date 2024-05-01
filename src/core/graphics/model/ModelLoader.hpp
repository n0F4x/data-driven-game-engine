#pragma once

#include <filesystem>

#include <tl/optional.hpp>

#include "core/cache/Cache.hpp"
#include "core/cache/Handle.hpp"

#include "Model.hpp"

namespace core::graphics {

class ModelLoader {
public:
    ModelLoader() noexcept = default;
    explicit ModelLoader(cache::Cache& t_cache) noexcept;

    [[nodiscard]]
    auto load_from_file(
        const std::filesystem::path& t_filepath,
        tl::optional<size_t>         t_scene_id = tl::nullopt
    ) noexcept -> tl::optional<cache::Handle<Model>>;

    [[nodiscard]]
    auto load_model(const fastgltf::Asset& t_asset, size_t t_scene_id) -> Model;

private:
    tl::optional<cache::Cache&> m_cache;
};

}   // namespace core::graphics
