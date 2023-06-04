#include <chrono>
#include <iostream>
#include <thread>

#include "app/App.hpp"
#include "schedule/Schedule.hpp"

auto main() -> int {
    using namespace std::chrono;

    try {
        simple_app::App::Builder{}
            .set_name("My game framework")
            .set_schedule(
                engine::Schedule::Builder{}
                    .add_stage(engine::Stage::Builder{}
                                   .add_system([](auto t_controller) {
                                       std::this_thread::sleep_for(500ms);
                                       std::cout << "Stage 1 - first\n";
                                       t_controller.quit();
                                   })
                                   .add_system([](auto t_controller) {
                                       std::cout << "Stage 1 - second\n";
                                       t_controller.quit();
                                   }))
                    .add_stage(engine::Stage::Builder{}.add_system(
                        [](auto t_controller) {
                            std::cout << "Stage 2\n";
                            t_controller.quit();
                        }))
                    .build())
            .build()
            .run();
    } catch (const std::exception&) {
        std::cout << "Oops... Something went wrong!\n";
    }
}
