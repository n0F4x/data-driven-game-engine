module;

#include <tuple>
#include <utility>

#include <vk_mem_alloc.h>

module modules.renderer.resources.Buffer;

import utility.tuple.tuple_drop_back;

import modules.renderer.base.allocator.Allocator;

modules::renderer::resources::Buffer::Buffer(
    const base::Allocator&         allocator,
    const vk::BufferCreateInfo&    buffer_create_info,
    const VmaAllocationCreateInfo& allocation_create_info
)
    : Buffer{ make_from(
          util::tuple_drop_back(
              allocator.create_buffer(buffer_create_info, allocation_create_info)
          )
      ) }
{}

auto modules::renderer::resources::Buffer::buffer() const -> const base::Buffer&
{
    return m_buffer;
}

auto modules::renderer::resources::Buffer::allocation() const -> const base::Allocation&
{
    return m_allocation;
}

auto modules::renderer::resources::Buffer::reset() noexcept -> void
{
    m_allocation.reset();
    m_buffer.reset();
}

modules::renderer::resources::Buffer::Buffer(
    base::Buffer&&     buffer,
    base::Allocation&& allocation
) noexcept
    : m_buffer{ std::move(buffer) },
      m_allocation{ std::move(allocation) }
{}

auto modules::renderer::resources::Buffer::make_from(
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
