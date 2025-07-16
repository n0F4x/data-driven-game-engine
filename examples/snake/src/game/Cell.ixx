module;

#include <SFML/Graphics.hpp>

export module snake.game.Cell;

import snake.game.Position;

namespace game {

export struct Cell {
    Position           position;
    sf::RectangleShape shape;
};

}   // namespace game
