#include "ResourceManager.hpp"

namespace engine::plugins {

auto ResourceManager::setup(App::Context& t_context) noexcept -> void
{
    t_context.emplace<resource_manager::ResourceManager>();
}

}   // namespace engine::plugins
