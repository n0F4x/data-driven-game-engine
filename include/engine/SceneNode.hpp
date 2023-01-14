#pragma once

#include <vector>
#include <memory>


class SceneNode {
    std::vector<std::shared_ptr<SceneNode>> children;
};
