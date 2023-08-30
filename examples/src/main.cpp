#include <iostream>

#include <engine/app/App.hpp>
using namespace engine;

auto run(App::Renderer&, App::Window&) noexcept -> void {}

auto main() noexcept -> int
{
    auto result{ App::create()
                     .set_window(App::Window::create()
                                     .set_video_mode(sf::VideoMode{ 450u, 600u }
                                     )
                                     .set_title("My window")
                                     .set_style(sf::Style::Default))
                     .build_and_run(run) };

    std::cout << (result == Result::eSuccess ? "Success" : "Failure") << '\n';
}
