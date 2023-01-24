#pragma once

#include <memory>
#include <vector>

#include "engine/SceneNode.hpp"

class Scene;

class SceneGraph {
    public:
    [[nodiscard]] auto make_scene() const -> Scene;

    private:
    std::vector<std::shared_ptr<SceneNode>> sceneNodes;
};
