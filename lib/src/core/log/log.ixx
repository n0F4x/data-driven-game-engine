module;

#include <source_location>
#include <string_view>

export module core.log.log;

import core.log.Level;

namespace core::log {

export auto log(std::source_location location, Level level, std::string_view message) -> void;

}   // namespace core::log
