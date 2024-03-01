#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "engine/renderer/material_system/Texture.hpp"

#include "Scene.hpp"

namespace engine::renderer {

class SceneManager {
public:
    auto emplace_scene(size_t t_id, auto&&... t_args) -> Scene&;

    auto load_texture(std::filesystem::path t_filepath) -> Texture&;

private:
    std::unordered_map<std::filesystem::path, Texture> m_textures;

    std::vector<size_t> m_ids;
    std::vector<Scene>  m_scenes;
};

}   // namespace engine::renderer

#include "SceneManager.inl"
