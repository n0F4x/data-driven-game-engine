module;

#include <random>

#include <SFML/Graphics.hpp>

module snake.game.tasks.spawn_apple;

import ddge.modules.assets.Handle;
import ddge.modules.ecs;
import ddge.modules.time.FixedTimer;

import ddge.modules.scheduler.accessors.assets;
import ddge.modules.scheduler.accessors.ecs;
import ddge.modules.scheduler.primitives.as_task;

import snake.assets.TextureLoader;
import snake.game.Apple;
import snake.game.Cell;
import snake.game.Position;
import snake.game.Snake;

using namespace ddge::ecs::query_filter_tags;

using namespace ddge::scheduler::accessors::ecs;

using CachedTextureLoader =
    ddge::scheduler::accessors::assets::Cached<assets::TextureLoader>;

auto game::tasks::spawn_apple() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::as_task(
        +[](const CachedTextureLoader texture_loader, const Registry registry) -> void {
            static std::random_device random_device;
            static std::mt19937       random_engine{ random_device() };

            std::vector<ddge::ecs::ID> empty_cell_ids;
            ddge::ecs::query(
                registry.get(),
                [&empty_cell_ids](
                    const ddge::ecs::ID id, With<Cell>, Without<Snake>, Without<Apple>
                ) -> void { empty_cell_ids.push_back(id); }
            );

            std::uniform_int_distribution distribution{ 0uz, empty_cell_ids.size() - 1uz };

            const ddge::ecs::ID new_apple_id{
                empty_cell_ids.at(distribution(random_engine))
            };

            const ddge::assets::Handle texture_handle{
                texture_loader->load("PixelApple.png")
            };

            Cell& cell{ registry->get_single<Cell>(new_apple_id) };
            cell.shape.setTexture(texture_handle.get());

            registry->insert(new_apple_id, Apple{});
        }
    );
}
