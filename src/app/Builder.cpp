#include "App.hpp"

auto App::Builder::build() -> App
{
    App app;

    for (const auto& plugin : m_plugins) {
        std::invoke(plugin, app);
    }

    return app;
}
