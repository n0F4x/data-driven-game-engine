#include "Plugin.hpp"

#include <spdlog/spdlog.h>

#include "SceneManager.hpp"

namespace engine::scene_graph {

auto Plugin::operator()(Store& t_store) noexcept -> void
{
    t_store.emplace<SceneManager>();

    SPDLOG_TRACE("Added SceneGraph plugin");
}

}   // namespace engine::scene_graph
