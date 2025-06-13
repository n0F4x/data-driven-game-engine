module;

#include <string>
#include <utility>

#include <SFML/Window.hpp>

export module snake.window.plugin;

import core.app.decays_to_builder_c;
import core.app.extended_with_c;
import core.time.FixedTimer;

import snake.window.display_rate;
import snake.window.Settings;
import snake.window.Window;

import extensions.Events;
import extensions.Resources;

namespace window {

export constexpr inline auto make_plugin = [](Settings settings) {
    return [settings = std::move(settings)]<core::app::decays_to_builder_c Builder_T>(
               Builder_T&& builder
           ) {
        static_assert(core::app::
                          extended_with_c<Builder_T, extensions::ResourcesTag>);
        static_assert(core::app::extended_with_c<Builder_T, extensions::EventsTag>);

        return std::forward<Builder_T>(builder)
            .use_resource(settings)
            .use_resource(
                Window{ sf::VideoMode{ sf::Vector2u{ settings.width, settings.height } },
                        std::string{ settings.title } }
            )
            .use_resource(core::time::FixedTimer<display_rate>{})
            .template register_event<sf::Event>();
    };
};

}   // namespace window
