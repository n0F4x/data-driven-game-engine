#include "RenderModelLoader.hpp"

namespace core::renderer {

RenderModelLoader::RenderModelLoader(
    const vk::Device           t_device,
    const Allocator&           t_allocator,
    const tl::optional<cache::Cache&> t_cache
) noexcept
    : m_device{ t_device },
      m_allocator{ t_allocator },
      m_cache{ t_cache }
{}


}   // namespace core::renderer

// auto RenderModelLoader::load(
//     const Model&      t_model,
//     vk::CommandBuffer t_transfer_command_buffer
//) -> tl::optional<RenderModel>
//{}
