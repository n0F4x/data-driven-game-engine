#include "framework/SceneGraph.hpp"

#include "framework/Scene.hpp"

namespace fw {

auto SceneGraph::make_scene() const -> Scene {
    return Scene{};
}

}   // namespace fw
