#include "SceneGraph.hpp"

#include "scene_graph/SceneManager.hpp"

namespace engine::plugins {

auto SceneGraph::operator()(App::Context& t_context) noexcept -> void
{
    t_context.emplace<scene_graph::SceneManager>();

    SPDLOG_TRACE("Added SceneGraph plugin");
}

}   // namespace engine::plugins
