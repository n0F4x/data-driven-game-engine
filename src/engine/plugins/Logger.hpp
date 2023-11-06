#pragma once

#include <spdlog/common.h>

#include "engine/app/Plugin.hpp"

namespace engine::logger {

enum class Level {
    eTrace    = SPDLOG_LEVEL_TRACE,
    eDebug    = SPDLOG_LEVEL_DEBUG,
    eInfo     = SPDLOG_LEVEL_INFO,
    eWarn     = SPDLOG_LEVEL_WARN,
    eError    = SPDLOG_LEVEL_ERROR,
    eCritical = SPDLOG_LEVEL_CRITICAL,
    eOff      = SPDLOG_LEVEL_OFF
};

}   // namespace engine::logger

namespace engine::plugins {

class Logger {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(
        App::Store& t_store,
        logger::Level t_log_level = logger::Level::eDebug
    ) noexcept -> void;
};

static_assert(PluginConcept<Logger, logger::Level>);

}   // namespace engine::plugins
