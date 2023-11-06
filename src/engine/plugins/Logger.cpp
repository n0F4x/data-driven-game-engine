#include "Logger.hpp"

#include <spdlog/spdlog.h>

namespace engine::plugins {

auto Logger::operator()(App::Store&, logger::Level t_log_level) noexcept -> void
{
    spdlog::set_level(static_cast<spdlog::level::level_enum>(t_log_level));

    SPDLOG_TRACE("Added Logger plugin");
}

}   // namespace engine::plugins
