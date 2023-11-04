#include "AssetManager.hpp"

#include <spdlog/spdlog.h>

#include "engine/plugins/asset_manager/AssetRegistry.hpp"

namespace engine::plugins {

auto AssetManager::operator()(App::Store& t_store) noexcept -> void
{
    t_store.emplace<asset_manager::AssetRegistry>();

    SPDLOG_TRACE("Added AssetRegistry plugin");
}

}   // namespace engine::plugins

// namespace engine::plugins
