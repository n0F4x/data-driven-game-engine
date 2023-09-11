#pragma once

#include "engine/common/app/Plugin.hpp"

namespace engine::plugins {

class Renderer {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto set_context(App::Context& t_context) const noexcept -> void;
};

static_assert(PluginConcept<Renderer>);

}   // namespace engine::plugins
