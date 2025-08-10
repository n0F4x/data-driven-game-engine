module;

#include <source_location>
#include <string_view>

export module ddge.modules.log.log;

import ddge.modules.log.Level;

namespace ddge::log {

export auto log(std::source_location location, Level level, std::string_view message)
    -> void;

}   // namespace ddge::log
