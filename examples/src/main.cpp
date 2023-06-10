#include <engine/app/App.hpp>

using App = engine::App;

auto run(App::Renderer&, App::Window&) -> void {}

auto main() -> int {
    auto my_app =
        App::create()
            .set_renderer(App::Renderer::create())
            .set_runner(run)
            .set_window(App::Window::create()
                            .set_video_mode(sf::VideoMode{ 450u, 600u })
                            .set_title("My window")
                            .set_style(sf::Style::Default))
            .build();
    my_app.run();
}
