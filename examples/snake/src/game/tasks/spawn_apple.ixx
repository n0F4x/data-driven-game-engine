module;

#include <random>

#include <SFML/Graphics.hpp>

export module snake.game.spawn_apple;

import modules.assets.Handle;
import modules.ecs;
import modules.time.FixedTimer;

import modules.scheduler.accessors.assets.Cached;
import modules.scheduler.accessors.ecs.Registry;

import snake.assets.TextureLoader;
import snake.game.Apple;
import snake.game.Cell;
import snake.game.Position;
import snake.game.Snake;

using namespace modules::ecs::query_parameter_tags;

using namespace modules::scheduler::accessors::ecs;

using CachedTextureLoader =
    modules::scheduler::accessors::assets::Cached<assets::TextureLoader>;

namespace game {

export auto spawn_apple(CachedTextureLoader texture_loader, Registry registry) -> void;

}   // namespace game

module :private;

auto game::spawn_apple(const CachedTextureLoader texture_loader, const Registry registry)
    -> void
{
    static std::random_device random_device;
    static std::mt19937       random_engine{ random_device() };

    std::vector<modules::ecs::ID> empty_cell_ids;
    modules::ecs::query(
        registry.get(),
        [&empty_cell_ids](const modules::ecs::ID id, With<Cell>, Without<Snake>, Without<Apple>) {
            empty_cell_ids.push_back(id);
        }
    );

    std::uniform_int_distribution distribution{ 0uz, empty_cell_ids.size() - 1uz };

    const modules::ecs::ID new_apple_id{ empty_cell_ids.at(distribution(random_engine)) };

    const modules::assets::Handle texture_handle{ texture_loader->load("PixelApple.png") };

    Cell& cell{ registry->get_single<Cell>(new_apple_id) };
    cell.shape.setTexture(texture_handle.get());

    registry->insert(new_apple_id, Apple{});
}
