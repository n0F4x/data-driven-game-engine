#pragma once

#include "engine/app/Plugin.hpp"

namespace engine::plugins {

class SceneGraph {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Context& t_context) noexcept -> void;
};

static_assert(PluginConcept<SceneGraph>);

}   // namespace engine::plugins
