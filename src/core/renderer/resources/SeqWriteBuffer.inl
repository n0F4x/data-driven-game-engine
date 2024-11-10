#pragma once

#include "core/renderer/base/allocator/Allocator.hpp"
#include "core/renderer/base/resources/copy_operations.hpp"

template <typename T>
core::renderer::resources::SeqWriteBuffer<T>::SeqWriteBuffer(
    const base::Allocator&      allocator,
    const vk::BufferCreateInfo& buffer_create_info,
    const void*                 data
)
    : SeqWriteBuffer{ make_from(
          allocator.create_buffer(
              buffer_create_info,
              VmaAllocationCreateInfo{
                  .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
                         | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                  .usage = VMA_MEMORY_USAGE_AUTO,
              }
          ),
          data
      ) }
{}

template <typename T>
auto core::renderer::resources::SeqWriteBuffer<T>::get() const noexcept -> vk::Buffer
{
    return m_buffer.get();
}

template <typename T>
template <size_t E>
auto core::renderer::resources::SeqWriteBuffer<T>::set(
    std::span<const T, E> data,
    const size_t          offset
) const -> void
{
    assert(data.size_bytes() <= size_bytes() - offset * sizeof(T));
    base::copy(
        data.data(),
        base::CopyRegion{ .allocation = m_allocation, .offset = offset * sizeof(T) },
        sizeof(T)
    );
}

template <typename T>
auto core::renderer::resources::SeqWriteBuffer<T>::set(const T& data, const size_t offset)
    const -> void
{
    assert(sizeof(T) <= size_bytes() - offset * sizeof(T));
    base::copy(
        std::addressof(data),
        base::CopyRegion{ .allocation = m_allocation, .offset = offset * sizeof(T) },
        sizeof(T)
    );
}

template <typename T>
auto core::renderer::resources::SeqWriteBuffer<T>::size() const noexcept -> size_t
{
    return size_bytes() / sizeof(T);
}

template <typename T>
auto core::renderer::resources::SeqWriteBuffer<T>::size_bytes() const noexcept -> size_t
{
    return m_buffer.size();
}

template <typename T>
auto core::renderer::resources::SeqWriteBuffer<T>::make_from(
    std::tuple<base::Buffer, base::Allocation, VmaAllocationInfo>&& tuple,
    const void*                                                     data
) noexcept -> SeqWriteBuffer
{
    if (data != nullptr) {
        base::copy(
            data,
            base::CopyRegion{ .allocation = std::get<base::Allocation>(tuple) },
            std::get<base::Buffer>(tuple).size()
        );
    }

    using RValueTuple = std::tuple<base::Buffer, base::Allocation, VmaAllocationInfo>&&;

    return SeqWriteBuffer{ std::get<base::Buffer>(static_cast<RValueTuple>(tuple)),
                           std::get<base::Allocation>(static_cast<RValueTuple>(tuple)) };
}

template <typename T>
core::renderer::resources::SeqWriteBuffer<T>::SeqWriteBuffer(
    base::Buffer&&     buffer,
    base::Allocation&& allocation
)
    : m_buffer{ std::move(buffer) },
      m_allocation{ std::move(allocation) }
{}
