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
            .add_state(fw::State::create<"MyState"_hs>()
                           .on_enter(+[] { std::cout << "Entered\n"; })
                           .on_exit(exited))
            .add_stage(engine::Stage::create()
                           .add_system(+[](engine::Controller& controller) {
                               std::this_thread::sleep_for(500ms);
                               std::cout << "Stage 1 - first\n";
                               controller.quit();
                           })
                           .add_system(+[](engine::Controller& controller) {
                               std::cout << "Stage 1 - second\n";
                               controller.quit();
                           }))
            .add_stage(engine::Stage::create().add_system(+[](engine::Controller& controller) {
                std::cout << "Stage 2\n";
                controller.quit();
            }))
            .build()
            .run();
    } catch (const std::exception&) {
        std::cout << "Oops... Something went wrong!\n";
    }
}
