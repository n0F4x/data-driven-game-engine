#include <chrono>
#include <iostream>
#include <thread>

#include <entt/core/hashed_string.hpp>

#include "app/App.hpp"

void exited() {
    std::cout << "Exited\n";
}

auto main() -> int {
    using namespace entt::literals;
    using namespace std::chrono;
    using App = app::App;

    try {
        App::Builder{}
            .set_name("My game framework")
            .set_state_machine(App::StateMachine::Builder{}.add_state(
                App::StateMachine::State::Builder{}
                    .set_id("MyState"_hs)
                    .on_enter(+[] { std::cout << "Entered\n"; })
                    .on_exit(exited),
                true))
            .set_schedule(
                App::Schedule::Builder{}
                    .add_stage(App::Schedule::Stage::Builder{}
                                   .add_system([](auto& t_controller) {
                                       std::this_thread::sleep_for(500ms);
                                       std::cout << "Stage 1 - first\n";
                                       t_controller.quit();
                                   })
                                   .add_system([](auto& t_controller) {
                                       std::cout << "Stage 1 - second\n";
                                       t_controller.quit();
                                   }))
                    .add_stage(App::Schedule::Stage::Builder{}.add_system(
                        [](auto& t_controller) {
                            std::cout << "Stage 2\n";
                            t_controller.quit();
                        })))
            .build()
            .run();
    } catch (const std::exception&) {
        std::cout << "Oops... Something went wrong!\n";
    }
}
