module;

#include <source_location>
#include <string_view>

export module modules.log.log;

import modules.log.Level;

namespace modules::log {

export auto log(std::source_location location, Level level, std::string_view message)
    -> void;

}   // namespace modules::log
