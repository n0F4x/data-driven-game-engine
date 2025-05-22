#pragma once

#include <source_location>

#include "log_level_definitions.hpp"

#define ENGINE_LOG_CRITICAL(message) \
    core::log::log(std::source_location::current(), core::log::Level::eCritical, message)

#define ENGINE_LOG_ERROR(message) \
    core::log::log(std::source_location::current(), core::log::Level::eError, message)

#define ENGINE_LOG_WARNING(message) \
    core::log::log(std::source_location::current(), core::log::Level::eWarning, message)

#define ENGINE_LOG_INFO(message) \
    core::log::log(std::source_location::current(), core::log::Level::eInfo, message)

#define ENGINE_LOG_DEBUG(message) \
    core::log::log(std::source_location::current(), core::log::Level::eDebug, message)

#define ENGINE_LOG_TRACE(message) \
    core::log::log(std::source_location::current(), core::log::Level::eTrace, message)
