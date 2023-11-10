#include "Plugin.hpp"

#include <spdlog/spdlog.h>

#include "AssetRegistry.hpp"

namespace engine::asset_manager {

auto Plugin::operator()(Store& t_store) noexcept -> void
{
    t_store.emplace<AssetRegistry>();

    SPDLOG_TRACE("Added AssetRegistry plugin");
}

}   // namespace engine::asset_manager
