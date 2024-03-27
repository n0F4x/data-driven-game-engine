#include "Logger.hpp"

#include <spdlog/spdlog.h>

namespace plugins {

auto Logger::operator()(app::App::Builder&, Level t_log_level) const noexcept -> void
{
    spdlog::set_level(static_cast<spdlog::level::level_enum>(t_log_level));

    SPDLOG_TRACE("Added Logger plugin");
}

}   // namespace plugins
