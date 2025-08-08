#pragma once

#include <source_location>

#include "log_level_definitions.hpp"

#define ENGINE_LOG_CRITICAL(message) \
    modules::log::log(std::source_location::current(), modules::log::Level::eCritical, message)

#define ENGINE_LOG_ERROR(message) \
    modules::log::log(std::source_location::current(), modules::log::Level::eError, message)

#define ENGINE_LOG_WARNING(message) \
    modules::log::log(std::source_location::current(), modules::log::Level::eWarning, message)

#define ENGINE_LOG_INFO(message) \
    modules::log::log(std::source_location::current(), modules::log::Level::eInfo, message)

#define ENGINE_LOG_DEBUG(message) \
    modules::log::log(std::source_location::current(), modules::log::Level::eDebug, message)

#define ENGINE_LOG_TRACE(message) \
    modules::log::log(std::source_location::current(), modules::log::Level::eTrace, message)
