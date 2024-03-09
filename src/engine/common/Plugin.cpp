#include "Plugin.hpp"

#include <spdlog/spdlog.h>

#include "engine/common/Cache.hpp"

namespace engine::common {

auto Plugin::operator()(Store& t_store) noexcept -> void
{
    t_store.emplace<Cache>();

    SPDLOG_TRACE("Added Common plugin");
}

}   // namespace engine::common
