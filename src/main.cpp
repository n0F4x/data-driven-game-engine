#include <chrono>
#include <iostream>
#include <thread>

#include <entt/core/hashed_string.hpp>

#include "engine/prelude.hpp"

void exited() noexcept {
    std::cout << "Exited\n";
}

auto main() -> int {
    using namespace entt::literals;
    using namespace std::chrono;

    try {
        engine::App::create()
            .set_name("My game framework")
            .add_state(fw::State::create()
                           .set_id("MyState"_hs)
                           .on_enter(+[] { std::cout << "Entered\n"; })
                           .on_exit(exited),
                       true)
            .add_stage(engine::Stage::create()
                           .add_system(+[](engine::Controller& t_controller) {
                               std::this_thread::sleep_for(500ms);
                               std::cout << "Stage 1 - first\n";
                               t_controller.quit();
                           })
                           .add_system(+[](engine::Controller& t_controller) {
                               std::cout << "Stage 1 - second\n";
                               t_controller.quit();
                           }))
            .add_stage(engine::Stage::create().add_system(
                +[](engine::Controller& t_controller) {
                    std::cout << "Stage 2\n";
                    t_controller.quit();
                }))
            .build()
            .run();
    } catch (const std::exception&) {
        std::cout << "Oops... Something went wrong!\n";
    }
}
