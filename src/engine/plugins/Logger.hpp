#pragma once

#include "engine/app/Plugin.hpp"

namespace engine::plugins {

class Logger {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    static auto setup(App::Context& t_context) noexcept -> void;
};

static_assert(PluginConcept<Logger>);

}   // namespace engine::plugins
