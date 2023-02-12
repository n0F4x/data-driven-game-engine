#include <chrono>
#include <iostream>
#include <thread>

#include "app/App.hpp"

auto main() -> int {
    using namespace std::chrono;
    using App = app::App;
    using Schedule = app::Schedule<App::AppView>;

    try {
        App::Builder{}
            .set_name("My game framework")
            .set_schedule<Schedule>(
                Schedule::Builder{}
                    .add_stage(Schedule::Stage::Builder{}
                                   .add_system([](auto t_controller) {
                                       std::this_thread::sleep_for(500ms);
                                       std::cout << "Stage 1 - first\n";
                                       t_controller.quit();
                                   })
                                   .add_system([](auto t_controller) {
                                       std::cout << "Stage 1 - second\n";
                                       t_controller.quit();
                                   }))
                    .add_stage(Schedule::Stage::Builder{}.add_system(
                        [](auto t_controller) {
                            std::cout << "Stage 2\n";
                            t_controller.quit();
                        })))
            .build()
            .run();
    } catch (const std::exception&) {
        std::cout << "Oops... Something went wrong!\n";
    }
}
