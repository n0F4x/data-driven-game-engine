module;

#include <optional>

export module snake.game.eat_apple;

import core.ecs;

import extensions.scheduler.accessors.ecs.Registry;

import snake.game.Apple;
import snake.game.Cell;
import snake.game.Snake;
import snake.game.SnakeHead;

using namespace core::ecs::query_parameter_tags;
using namespace extensions::scheduler::accessors::ecs;

namespace game {

export inline constexpr auto eat_apple = [](const Registry registry) {
    std::optional<core::ecs::ID> eaten_cell_id;

    core::ecs::query(
        registry.get(),
        [&eaten_cell_id](const core::ecs::ID id, With<SnakeHead>, With<Apple>) {
            eaten_cell_id = id;
        }
    );

    if (eaten_cell_id.has_value()) {
        core::ecs::query(registry.get(), [](Snake& snake) { snake.charge++; });

        registry->remove<Apple>(*eaten_cell_id);
        registry->get_single<Cell>(*eaten_cell_id).shape.setTexture(nullptr);
    }
};

}   // namespace game
