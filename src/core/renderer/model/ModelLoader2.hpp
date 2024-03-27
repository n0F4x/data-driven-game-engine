#pragma once

#include <filesystem>

#include <tl/optional.hpp>

#include "core/common/Cache.hpp"
#include "core/common/Handle.hpp"

#include "Model.hpp"

namespace core::renderer {

class ModelLoader {
public:
    ModelLoader() noexcept = default;
    explicit ModelLoader(Cache& t_cache) noexcept;

    [[nodiscard]] auto load_from_file(
        const std::filesystem::path& t_filepath,
        tl::optional<size_t>         t_scene_id
    ) noexcept -> tl::optional<Handle<Model>>;

private:
    tl::optional<Cache&> m_cache;
};

}   // namespace core::renderer
