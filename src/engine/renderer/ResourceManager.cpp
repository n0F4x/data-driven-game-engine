#include "ResourceManager.hpp"

namespace engine::renderer {

ResourceManager::ResourceManager(const Allocator& t_allocator) noexcept
    : m_allocator{ t_allocator }
{}

}   // namespace engine::renderer
