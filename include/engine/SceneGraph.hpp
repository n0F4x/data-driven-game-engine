#pragma once

#include <vector>
#include <memory>

#include "engine/SceneNode.hpp"

class Scene;


class SceneGraph {
public:
    [[nodiscard]] auto make_scene() -> Scene;

private:
    std::vector<std::shared_ptr<SceneNode>> sceneNodes;
};
