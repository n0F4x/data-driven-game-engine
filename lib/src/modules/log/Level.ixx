module;

#include "modules/log/log_level_definitions.hpp"

export module ddge.modules.log.Level;

namespace ddge::log {

export enum struct Level {
    eOff      = ENGINE_LOG_LEVEL_OFF,
    eCritical = ENGINE_LOG_LEVEL_CRITICAL,
    eError    = ENGINE_LOG_LEVEL_ERROR,
    eWarning  = ENGINE_LOG_LEVEL_WARNING,
    eInfo     = ENGINE_LOG_LEVEL_INFO,
    eDebug    = ENGINE_LOG_LEVEL_DEBUG,
    eTrace    = ENGINE_LOG_LEVEL_TRACE,
};

}   // namespace ddge::log
