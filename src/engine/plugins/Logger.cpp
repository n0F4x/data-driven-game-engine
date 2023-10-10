#include "Logger.hpp"

#include <spdlog/spdlog.h>

namespace engine::plugins {

auto Logger::operator()(App::Context&) noexcept -> void
{
    spdlog::set_level(spdlog::level::trace);

    SPDLOG_TRACE("Added Logger plugin");
}

}   // namespace engine::plugins
