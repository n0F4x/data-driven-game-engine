#include "SceneBuilder.hpp"

namespace engine::renderer {

SceneBuilder::SceneBuilder(ResourceManager& t_resource_manager) noexcept
    : m_resource_manager{ t_resource_manager }
{}

}   // namespace engine::renderer
