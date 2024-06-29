#pragma once

#include <spdlog/common.h>

#include "app/Plugin.hpp"

namespace plugins {

class Logger {
public:
    enum class Level {
        eTrace    = SPDLOG_LEVEL_TRACE,
        eDebug    = SPDLOG_LEVEL_DEBUG,
        eInfo     = SPDLOG_LEVEL_INFO,
        eWarn     = SPDLOG_LEVEL_WARN,
        eError    = SPDLOG_LEVEL_ERROR,
        eCritical = SPDLOG_LEVEL_CRITICAL,
        eOff      = SPDLOG_LEVEL_OFF
    };

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Builder& t_builder, Level t_log_level = Level::eDebug)
        const noexcept -> void;
};

static_assert(PluginConcept<Logger, Logger::Level>);

}   // namespace plugins
