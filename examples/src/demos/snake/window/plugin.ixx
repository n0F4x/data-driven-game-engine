module;

#include <string>
#include <utility>

#include <SFML/Window.hpp>

export module snake.window.plugin;

import app.decays_to_builder_c;
import app.has_plugins_c;

import core.time.FixedTimer;

import snake.window.display_rate;
import snake.window.Settings;
import snake.window.Window;

import plugins.events;
import plugins.resources;

namespace window {

export constexpr inline auto make_plugin = [](Settings settings) {
    return [settings = std::move(settings)]<app::decays_to_builder_c Builder_T>(
               Builder_T&& builder
           ) {
        static_assert(app::
                          has_plugins_c<Builder_T, plugins::ResourcesTag>);
        static_assert(app::has_plugins_c<Builder_T, plugins::EventsTag>);

        return std::forward<Builder_T>(builder)
            .insert_resource(settings)
            .insert_resource(
                Window{ sf::VideoMode{ sf::Vector2u{ settings.width, settings.height } },
                        std::string{ settings.title } }
            )
            .insert_resource(core::time::FixedTimer<display_rate>{})
            .template register_event<sf::Event>();
    };
};

}   // namespace window
