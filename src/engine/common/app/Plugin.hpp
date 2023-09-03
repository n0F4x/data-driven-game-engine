#pragma once

#include "App.hpp"

namespace engine {

template <typename Plugin>
concept PluginConcept = requires(Plugin t_plugin, App::Context& t_context) {
    {
        t_plugin.set_context(t_context)
    };
};

}   // namespace engine
