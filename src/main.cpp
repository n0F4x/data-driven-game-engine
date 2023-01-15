#include <chrono>
#include <iostream>
#include <thread>

#include <entt/core/hashed_string.hpp>

#include "prelude.hpp"

using namespace entt::literals;


void exited() noexcept {
    std::cout << "Exited\n";
}

auto main() -> int {
    try {
        App::create()
            .set_name("My game engine")
            .add_state(State::create<"MyState"_hs>()
                .on_enter(+[] { std::cout << "Entered\n"; })
                .on_exit(exited))
            .add_stage(Stage::create()
                .add_system(+[](Controller& controller) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    std::cout << "Stage 1 - first\n";
                    controller.quit(); })
                .add_system(+[](Controller& controller) {
                    std::cout << "Stage 1 - second\n";
                    controller.quit(); }))
            .add_stage(Stage::create()
                .add_system(+[](Controller& controller) {
                    std::cout << "Stage 2\n";
                    controller.quit(); }))
            .build().run();
    } catch (const std::exception&) {
        std::cout << "Oops... Something went wrong!\n";
    }
}
