#include <engine/app/App.hpp>

using namespace engine;

auto run(Renderer&, Window&) noexcept -> void {}

auto main() noexcept -> int
{
    App::create()
        .set_window(Window::create()
                        .set_video_mode(sf::VideoMode{ 450u, 600u })
                        .set_title("My window")
                        .set_style(sf::Style::Default))
        .build_and_run(run);
}
