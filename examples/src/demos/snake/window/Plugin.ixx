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
        constexpr static auto ensure_resource_manager = []<typename XBuilder_T>(
                                                            XBuilder_T&& x_builder
                                                        ) {
            if constexpr (!core::app::
                              extended_with_c<XBuilder_T, extensions::ResourceManagerTag>)
            {
                return std::forward<XBuilder_T>(x_builder).extend_with(
                    extensions::ResourceManager{}
                );
            }
            else {
                return std::forward<XBuilder_T>(x_builder);
            }
        };

        constexpr static auto register_events = []<typename XBuilder_T>(
                                                    XBuilder_T&& x_builder
                                                ) {
            constexpr static auto ensure_event_manager = [](XBuilder_T&& y_builder) {
                if constexpr (!core::app::
                                  extended_with_c<XBuilder_T, extensions::EventManager>)
                {
                    return std::forward<XBuilder_T>(y_builder).extend_with(
                        extensions::EventManager{}
                    );
                }
                else {
                    return std::forward<XBuilder_T>(y_builder);
                }
            };

            return ensure_event_manager(std::forward<XBuilder_T>(x_builder))
                .template register_event<events::CloseRequested>();
        };

        return register_events(ensure_resource_manager(std::forward<Builder_T>(builder))
                                   .use_resource(Window{ settings }));
    };
};

}   // namespace window
