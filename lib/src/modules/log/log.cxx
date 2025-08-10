module;

#include <source_location>
#include <string_view>
#include <utility>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "log_level_definitions.hpp"

module ddge.modules.log.log;

import ddge.modules.log.Level;

auto logger{ [] {
    auto result{ spdlog::stdout_color_mt("engine_internal") };
    result->set_level(spdlog::level::trace);
    return result;
}() };

[[nodiscard]]
constexpr auto convert(const ddge::log::Level level) -> spdlog::level::level_enum
{
    switch (level) {
        case ddge::log::Level::eOff:      return spdlog::level::off;
        case ddge::log::Level::eCritical: return spdlog::level::critical;
        case ddge::log::Level::eError:    return spdlog::level::err;
        case ddge::log::Level::eWarning:  return spdlog::level::warn;
        case ddge::log::Level::eInfo:     return spdlog::level::info;
        case ddge::log::Level::eDebug:    return spdlog::level::debug;
        case ddge::log::Level::eTrace:    return spdlog::level::trace;
        default:                          std::unreachable();
    }
}

auto ddge::log::log(
    const std::source_location location,
    const Level                level,
    const std::string_view     message
) -> void
{
    if (std::to_underlying(level) <= ENGINE_LOG_LEVEL) {
        logger->log(
            spdlog::source_loc{ location.file_name(),
                                static_cast<int>(location.line()),
                                location.function_name() },
            spdlog::level::level_enum{ convert(level) },
            message
        );
    }
}
