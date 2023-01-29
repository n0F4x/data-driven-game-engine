#pragma once

#include <memory>
#include <vector>

#include "framework/Scene.hpp"
#include "framework/SceneNode.hpp"

namespace fw {

class SceneGraph final {
public:
    using Scene = Scene;

    [[nodiscard]] auto make_scene() const -> Scene;

private:
    std::vector<std::shared_ptr<SceneNode>> m_sceneNodes;
};

}   // namespace fw
