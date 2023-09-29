#include "ResourceManager.hpp"

namespace engine::plugins {

auto ResourceManager::setup(App::Context& t_context) const noexcept -> void
{
    using namespace entt::literals;
    t_context.emplace<resource_manager::ResourceManager>(
        std::allocator<entt::id_type>{}
    );
}

}   // namespace engine::plugins
