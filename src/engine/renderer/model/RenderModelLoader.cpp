#include "RenderModelLoader.hpp"

namespace engine::renderer {

RenderModelLoader::RenderModelLoader(
    vk::Device           t_device,
    const Allocator&     t_allocator,
    tl::optional<Cache&> t_cache
) noexcept
    : m_device{ t_device },
      m_allocator{ t_allocator },
      m_cache{ t_cache }
{}

// auto RenderModelLoader::load(
//     const Model&      t_model,
//     vk::CommandBuffer t_transfer_command_buffer
//) -> tl::optional<RenderModel>
//{}

}   // namespace engine::renderer
