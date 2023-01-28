#include <chrono>
#include <iostream>
#include <thread>

#include <entt/core/hashed_string.hpp>

#include "engine/prelude.hpp"

void exited() {
    std::cout << "Exited\n";
}

auto main() -> int {
    using namespace entt::literals;
    using namespace std::chrono;
    using App = engine::App;

    try {
        App::Builder{}
            .set_name("My game framework")
            .set_state_machine(App::StateMachineType::Builder{}.add_state(
                App::StateMachineType::StateType::Builder{}
                    .set_id("MyState"_hs)
                    .on_enter(+[] { std::cout << "Entered\n"; })
                    .on_exit(exited),
                true))
            .set_schedule(
                App::ScheduleType::Builder{}
                    .add_stage(
                        App::ScheduleType::StageType::Builder{}
                            .add_system(
                                +[](App::ScheduleType::StageType::ControllerType&
                                        t_controller) {
                                    std::this_thread::sleep_for(500ms);
                                    std::cout << "Stage 1 - first\n";
                                    t_controller.quit();
                                })
                            .add_system(
                                +[](App::ScheduleType::StageType::ControllerType&
                                        t_controller) {
                                    std::cout << "Stage 1 - second\n";
                                    t_controller.quit();
                                }))
                    .add_stage(
                        App::ScheduleType::StageType::Builder{}.add_system(
                            +[](App::ScheduleType::StageType::ControllerType&
                                    t_controller) {
                                std::cout << "Stage 2\n";
                                t_controller.quit();
                            })))
            .build()
            .run();
    } catch (const std::exception&) {
        std::cout << "Oops... Something went wrong!\n";
    }
}
