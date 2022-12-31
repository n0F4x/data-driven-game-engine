#include <iostream>
#include <thread>
#include <chrono>

#include <entt/entt.hpp>

#include "engine.hpp"

using namespace entt::literals;


void exited() noexcept {
    std::cout << "Exited\n";
}

auto main() -> int {
    App::create()
        .set_name("My game engine")
        .add_state(State::create<"MyState"_hs>()
            .on_enter(+[] { std::cout << "Entered\n"; })
            .on_exit(exited))
        .add_stage(Stage::create()
            .add_system(+[](Controller& controller) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "Stage 1 - first\n";
                controller.quit(); })
            .add_system(+[](Controller& controller) {
                std::cout << "Stage 1 - second\n";
                controller.quit(); }))
        .add_stage(Stage::create()
            .add_system(+[](Controller& controller) { std::cout << "Stage 2\n"; controller.quit(); }))
        .build().run();
}
