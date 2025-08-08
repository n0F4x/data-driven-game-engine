module;

#include <source_location>
#include <string_view>
#include <utility>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "log_level_definitions.hpp"

module modules.log.log;

import modules.log.Level;

auto logger{ [] {
    auto result{ spdlog::stdout_color_mt("engine_internal") };
    result->set_level(spdlog::level::trace);
    return result;
}() };

[[nodiscard]]
constexpr auto convert(const modules::log::Level level) -> spdlog::level::level_enum
{
    switch (level) {
        case modules::log::Level::eOff:      return spdlog::level::off;
        case modules::log::Level::eCritical: return spdlog::level::critical;
        case modules::log::Level::eError:    return spdlog::level::err;
        case modules::log::Level::eWarning:  return spdlog::level::warn;
        case modules::log::Level::eInfo:     return spdlog::level::info;
        case modules::log::Level::eDebug:    return spdlog::level::debug;
        case modules::log::Level::eTrace:    return spdlog::level::trace;
        default:                          std::unreachable();
    }
}

auto modules::log::log(
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
