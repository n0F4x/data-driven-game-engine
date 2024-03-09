#include "Plugin.hpp"

#include <spdlog/spdlog.h>

#include "engine/app/Builder.hpp"
#include "engine/common/Cache.hpp"

namespace engine::common {

auto Plugin::operator()(App::Builder& t_builder) noexcept -> void
{
    t_builder.store().emplace<Cache>();

    SPDLOG_TRACE("Added Common plugin");
}

}   // namespace engine::common
