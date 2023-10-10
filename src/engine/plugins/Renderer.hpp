#pragma once

#include "engine/app/Plugin.hpp"

namespace engine::plugins {

class Renderer {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Context& t_context) noexcept -> void;
};

static_assert(PluginConcept<Renderer>);

}   // namespace engine::plugins
