#include "SceneGraph.hpp"

#include "scene_graph/SceneManager.hpp"

namespace engine::plugins {

auto SceneGraph::setup(App::Context& t_context) const noexcept -> void
{
    t_context.emplace<scene_graph::SceneManager>();
}

}   // namespace engine::plugins
