#pragma once

#include <memory>
#include <vector>

class SceneNode {
    std::vector<std::shared_ptr<SceneNode>> children;
};
