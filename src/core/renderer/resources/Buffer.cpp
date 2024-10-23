#include "Buffer.hpp"

#include <utility>

#include <core/renderer/base/allocator/Allocator.hpp>
#include <core/utility/tuple.hpp>

core::renderer::resources::Buffer::Buffer(
    const base::Allocator&         allocator,
    const vk::BufferCreateInfo&    buffer_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
)
    : Buffer{ make_from(core::utils::remove_last(
          allocator.create_buffer(buffer_create_info, allocation_create_info)
      )) }
{}

auto core::renderer::resources::Buffer::reset() noexcept -> void
{
    m_allocation.reset();
    base::Buffer::reset();
}

core::renderer::resources::Buffer::Buffer(
    base::Buffer&&     buffer,
    base::Allocation&& allocation
) noexcept
    : base::Buffer{ std::move(buffer) },
      m_allocation{ std::move(allocation) }
{}

auto core::renderer::resources::Buffer::make_from(
    std::tuple<base::Buffer, base::Allocation>&& tuple
) noexcept -> Buffer
{
    return Buffer{ std::get<base::Buffer>(
                       static_cast<std::tuple<base::Buffer, base::Allocation>&&>(tuple)
                   ),
                   std::get<base::Allocation>(
                       static_cast<std::tuple<base::Buffer, base::Allocation>&&>(tuple)
                   ) };
}
