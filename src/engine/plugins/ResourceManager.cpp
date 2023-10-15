#include "ResourceManager.hpp"

namespace engine::plugins {

auto ResourceManager::operator()(App::Context& t_context) noexcept -> void
{
    t_context.emplace<resource_manager::ResourceManager>();

    SPDLOG_TRACE("Added ResourceManager plugin");
}

}   // namespace engine::plugins
