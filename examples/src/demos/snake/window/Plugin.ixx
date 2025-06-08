module;

#include <utility>

export module demo.window.Plugin;

import core.app.decays_to_builder_c;
import core.app.extended_with_c;

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

        return builder.use_resource(Window{ std::move(settings) })
            .template register_event<events::CloseRequested>();
    };
};

}   // namespace window
