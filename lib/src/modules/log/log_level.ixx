module;

#include <magic_enum/magic_enum.hpp>

#include "modules/log/log_level_definitions.hpp"

export module ddge.modules.log.log_level;

import ddge.modules.log.Level;

namespace ddge::log {

export [[nodiscard]]
consteval auto log_level() -> Level
{
    static_assert(ENGINE_LOG_LEVEL < magic_enum::enum_count<Level>());
    return Level{ ENGINE_LOG_LEVEL };
}

}   // namespace ddge::log
