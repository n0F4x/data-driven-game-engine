#pragma once

#include <filesystem>

#include "Model.hpp"

namespace core::graphics {

class GltfLoader {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& t_filepath
    ) -> std::optional<Model>;

    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& t_filepath, size_t t_scene_id)
        -> std::optional<Model>;

private:
    [[nodiscard]]
    static auto load_model(const fastgltf::Asset& t_asset, size_t t_scene_id) -> Model;
};

}   // namespace core::graphics
