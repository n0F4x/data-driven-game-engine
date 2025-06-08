module;

#include <SFML/Graphics.hpp>

export module game.draw;

import core.ecs;

import extensions.scheduler.accessors.ecs.Query;
import extensions.scheduler.accessors.resources;

import game.Cell;

import demo.window.Window;

using namespace extensions::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

namespace game {

export constexpr inline auto draw =
    [](resources::Ref<window::Window>                   window,
       ecs::Query<const sf::RectangleShape, With<Cell>> cells) {
        cells.for_each([window](const sf::RectangleShape& shape) { window->draw(shape); });
    };

}   // namespace game
