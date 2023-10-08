#pragma once

#include "App.hpp"

namespace engine {

template <typename Plugin, typename... Args>
concept PluginConcept =
    requires(Plugin t_plugin, App::Context& t_context, Args&&... t_args) {
        {
            t_plugin.setup(t_context, std::forward<Args>(t_args)...)
        };
    };

}   // namespace engine
