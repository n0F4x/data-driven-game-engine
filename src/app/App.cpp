#include "App.hpp"

#include <spdlog/spdlog.h>

auto App::create() -> Builder
{
    SPDLOG_TRACE("Creating App...");
    return Builder{};
}

auto App::plugins() noexcept -> Store&
{
    return m_plugins;
}

auto App::plugins() const noexcept -> const Store&
{
    return m_plugins;
}
