#include "Logger.hpp"

#include <spdlog/spdlog.h>

namespace app::plugins {

auto Logger::operator()(App::Builder&, Level t_log_level) noexcept -> void
{
    spdlog::set_level(static_cast<spdlog::level::level_enum>(t_log_level));

    SPDLOG_TRACE("Added Logger plugin");
}

}   // namespace engine::logger
