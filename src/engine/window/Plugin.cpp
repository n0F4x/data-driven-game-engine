#include "Plugin.hpp"

#include <spdlog/spdlog.h>

#include "Window.hpp"

namespace engine::window {

auto Plugin::operator()(
    Store&               t_store,
    const sf::VideoMode& t_video_mode,
    std::string_view     t_title,
    Style                t_style
) -> void
{
    t_store.emplace<Window>(
        t_video_mode, t_title.data(), static_cast<sf::Uint32>(t_style)
    );
    SPDLOG_TRACE("Added Window plugin");
}

}   // namespace engine::window
