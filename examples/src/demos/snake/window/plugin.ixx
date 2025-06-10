module;

#include <utility>

export module snake.window.plugin;

import core.app.decays_to_builder_c;
import core.app.extended_with_c;
import core.time.FixedTimer;

import snake.window.display_rate;
import snake.window.events;
import snake.window.Settings;
import snake.window.Window;

import extensions.EventManager;
import extensions.ResourceManager;

namespace window {

export constexpr inline auto make_plugin = [](Settings settings) {
    return [settings = std::move(settings)]<core::app::decays_to_builder_c Builder_T>(
               Builder_T&& builder
           ) {
        static_assert(core::app::
                          extended_with_c<Builder_T, extensions::ResourceManagerTag>);
        static_assert(core::app::extended_with_c<Builder_T, extensions::EventManagerTag>);

        return std::forward<Builder_T>(builder)
            .use_resource(settings)
            .use_resource(Window{ std::move(settings) })
            .use_resource(core::time::FixedTimer<display_rate>{})
            .template register_event<events::CloseRequested>();
    };
};

}   // namespace window
