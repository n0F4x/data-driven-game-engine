#include "Plugin.hpp"

#include <spdlog/spdlog.h>

namespace engine::logger {

auto Plugin::operator()(Store&, Level t_log_level) noexcept -> void
{
    spdlog::set_level(static_cast<spdlog::level::level_enum>(t_log_level));

    SPDLOG_TRACE("Added Logger plugin");
}

}   // namespace engine::logger
