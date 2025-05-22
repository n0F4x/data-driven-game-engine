module;

#include "core/log/log_level_definitions.hpp"

export module core.log.Level;

namespace core::log {

export enum struct Level {
    eOff      = ENGINE_LOG_LEVEL_OFF,
    eCritical = ENGINE_LOG_LEVEL_CRITICAL,
    eError    = ENGINE_LOG_LEVEL_ERROR,
    eWarning  = ENGINE_LOG_LEVEL_WARNING,
    eInfo     = ENGINE_LOG_LEVEL_INFO,
    eDebug    = ENGINE_LOG_LEVEL_DEBUG,
    eTrace    = ENGINE_LOG_LEVEL_TRACE,
};

}   // namespace core::log
