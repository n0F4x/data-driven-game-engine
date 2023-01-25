#pragma once

#include <memory>
#include <vector>

namespace fw {

class SceneNode {
    std::vector<std::shared_ptr<SceneNode>> m_children;
};

}   // namespace fw
