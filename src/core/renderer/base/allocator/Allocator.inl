#pragma once

#include "core/renderer/memory/MappedBuffer.hpp"
#include "core/renderer/memory/SeqWriteBuffer.hpp"

#include "helpers.hpp"

namespace core::renderer {

template <typename T>
auto Allocator::allocate_seq_write_buffer(
    const vk::BufferCreateInfo& buffer_create_info,
    const void*                 data
) const -> SeqWriteBuffer<T>
{
    auto [allocation, buffer, allocation_info]{ details::create_mapped_buffer<
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT>(
        m_allocator.get(), buffer_create_info, data
    ) };

    return SeqWriteBuffer<T>{
        m_allocator.get(), allocation, buffer, buffer_create_info.size
    };
}

template <typename T>
auto Allocator::allocate_random_access_buffer(
    const vk::BufferCreateInfo& buffer_create_info,
    const void*                 data
) const -> RandomAccessBuffer<T>
{
    auto [allocation, buffer, allocation_info]{
        details::create_mapped_buffer<VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT>(
            m_allocator.get(), buffer_create_info, data
        )
    };

    return RandomAccessBuffer<T>{
        m_allocator.get(), allocation, buffer, buffer_create_info.size
    };
}

}   // namespace core::renderer
