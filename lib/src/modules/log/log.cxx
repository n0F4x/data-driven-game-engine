module;

#include <format>
#include <source_location>
#include <string_view>
#include <utility>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

module ddge.modules.log.log;

import ddge.modules.config.engine_name;
import ddge.modules.log.Level;
import ddge.modules.log.log_level;

namespace ddge::log {

[[nodiscard]]
constexpr auto convert(const Level level) -> spdlog::level::level_enum
{
    switch (level) {
        case Level::eOff:      return spdlog::level::off;
        case Level::eCritical: return spdlog::level::critical;
        case Level::eError:    return spdlog::level::err;
        case Level::eWarning:  return spdlog::level::warn;
        case Level::eInfo:     return spdlog::level::info;
        case Level::eDebug:    return spdlog::level::debug;
        case Level::eTrace:    return spdlog::level::trace;
        default:               std::unreachable();
    }
}

auto logger{ [] {
    auto result{
        spdlog::stdout_color_mt(std::format("{}:Internal", config::engine_name()))
    };
    result->set_level(convert(log_level()));
    result->set_pattern("%^[%n](%r) %l:%$ %v");
    return result;
}() };

auto log(
    const std::source_location location,
    const Level                level,
    const std::string_view     message
) -> void
{
    logger->log(
        spdlog::source_loc{ location.file_name(),
                            static_cast<int>(location.line()),
                            location.function_name() },
        spdlog::level::level_enum{ convert(level) },
        message
    );
}

}   // namespace ddge::log
