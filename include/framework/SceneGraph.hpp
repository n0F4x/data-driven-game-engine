#pragma once

#include <memory>
#include <vector>

#include "framework/SceneNode.hpp"

namespace fw {

class Scene;

class SceneGraph final {
public:
    [[nodiscard]] auto make_scene() const -> Scene;

private:
    std::vector<std::shared_ptr<SceneNode>> m_sceneNodes;
};

}   // namespace fw
