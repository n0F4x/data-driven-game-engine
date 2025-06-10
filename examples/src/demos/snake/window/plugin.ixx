module;

#include <utility>

export module demo.window.plugin;

import core.app.decays_to_builder_c;
import core.app.extended_with_c;
import core.time.FixedTimer;

import demo.window.display_rate;
import demo.window.events;
import demo.window.Settings;
import demo.window.Window;

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
