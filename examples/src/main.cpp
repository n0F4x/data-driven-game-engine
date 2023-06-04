#include "app/App.hpp"

using namespace app;

auto run(App::Window&) -> void {}

auto main() -> int {
    auto my_app =
        App::create()
            .set_window(App::Window::create()
                            .set_video_mode(sf::VideoMode{ 450u, 600u })
                            .set_title("My window")
                            .set_style(sf::Style::Default))
            .set_runner(run)
            .build();
    my_app.run();
}
