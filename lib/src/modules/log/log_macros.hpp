#pragma once

#include <source_location>

#include "log_level_definitions.hpp"

#define ENGINE_LOG_CRITICAL(message) \
    ddge::log::log(std::source_location::current(), ddge::log::Level::eCritical, message)

#define ENGINE_LOG_ERROR(message) \
    ddge::log::log(std::source_location::current(), ddge::log::Level::eError, message)

#define ENGINE_LOG_WARNING(message) \
    ddge::log::log(std::source_location::current(), ddge::log::Level::eWarning, message)

#define ENGINE_LOG_INFO(message) \
    ddge::log::log(std::source_location::current(), ddge::log::Level::eInfo, message)

#define ENGINE_LOG_DEBUG(message) \
    ddge::log::log(std::source_location::current(), ddge::log::Level::eDebug, message)

#define ENGINE_LOG_TRACE(message) \
    ddge::log::log(std::source_location::current(), ddge::log::Level::eTrace, message)
