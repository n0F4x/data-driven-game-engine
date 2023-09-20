#pragma once

#include "engine/app/Plugin.hpp"

namespace engine::plugins {

class Renderer {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto setup(App::Context& t_context) const noexcept -> void;
};

static_assert(PluginConcept<Renderer>);

}   // namespace engine::plugins
