#pragma once

#include <memory>
#include <vector>

namespace fw {

class SceneNode {
    std::vector<std::shared_ptr<SceneNode>> children;
};

}   // namespace fw
