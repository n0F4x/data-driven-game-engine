module;

#include "core/log/log.hpp"

export module core.config.logging;

namespace core::config::logging {

export enum Level {
    eTrace    = spdlog::level::trace,
    eDebug    = spdlog::level::debug,
    eInfo     = spdlog::level::info,
    eWarning  = spdlog::level::warn,
    eError    = spdlog::level::err,
    eCritical = spdlog::level::critical,
    eOff      = spdlog::level::off,
};

export auto set_level(Level level) -> void;

}   // namespace core::config::logging

module :private;

auto core::config::logging::set_level(const Level level) -> void
{
    core::log::internal::logger().flush();
    core::log::internal::logger().set_level(static_cast<spdlog::level::level_enum>(level));
}
