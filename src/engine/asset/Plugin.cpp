#include "Plugin.hpp"

#include <spdlog/spdlog.h>

#include "Registry.hpp"

namespace engine::asset {

auto Plugin::operator()(Store& t_store) noexcept -> void
{
    t_store.emplace<Registry>();

    SPDLOG_TRACE("Added Registry plugin");
}

}   // namespace engine::asset
