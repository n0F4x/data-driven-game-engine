#include "SceneGraph.hpp"

#include <spdlog/spdlog.h>

#include "scene_graph/SceneManager.hpp"

namespace engine::plugins {

auto SceneGraph::operator()(App::Store& t_store) noexcept -> void
{
    t_store.emplace<scene_graph::SceneManager>();

    SPDLOG_TRACE("Added SceneGraph plugin");
}

}   // namespace engine::plugins
