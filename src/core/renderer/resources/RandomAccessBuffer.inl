#pragma once

template <typename T>
core::renderer::resources::RandomAccessBuffer<T>::RandomAccessBuffer(
    const base::Allocator&      allocator,
    const vk::BufferCreateInfo& buffer_create_info,
    const void*                 data
)
    : RandomAccessBuffer{ make_from(
          allocator.create_buffer(
              buffer_create_info,
              VmaAllocationCreateInfo{
                  .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
                         | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                  .usage = VMA_MEMORY_USAGE_AUTO,
              }
          ),
          data
      ) }
{}

template <typename T>
auto core::renderer::resources::RandomAccessBuffer<T>::get() const noexcept -> vk::Buffer
{
    return m_buffer.get();
}

template <typename T>
template <size_t E>
auto core::renderer::resources::RandomAccessBuffer<T>::set(
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
auto core::renderer::resources::RandomAccessBuffer<T>::set(
    const T&     data,
    const size_t offset
) const -> void
{
    assert(sizeof(T) <= size_bytes() - offset * sizeof(T));
    base::copy(
        std::addressof(data),
        base::CopyRegion{ .allocation = m_allocation, .offset = offset * sizeof(T) },
        sizeof(T)
    );
}

template <typename T>
auto core::renderer::resources::RandomAccessBuffer<T>::size() const noexcept -> size_t
{
    return size_bytes() * sizeof(T);
}

template <typename T>
auto core::renderer::resources::RandomAccessBuffer<T>::size_bytes() const noexcept
    -> size_t
{
    return m_buffer.size();
}

template <typename T>
auto core::renderer::resources::RandomAccessBuffer<T>::make_from(
    std::tuple<base::Buffer, base::Allocation, VmaAllocationInfo>&& tuple,
    const void*                                                     data
) noexcept -> RandomAccessBuffer
{
    if (data != nullptr) {
        base::copy(
            data,
            base::CopyRegion{ .allocation = std::get<base::Allocation>(tuple) },
            std::get<base::Buffer>(tuple).size()
        );
    }

    using RValueTuple = std::tuple<base::Buffer, base::Allocation, VmaAllocationInfo>&&;

    return RandomAccessBuffer{ std::get<base::Buffer>(static_cast<RValueTuple>(tuple)),
                               std::get<base::Allocation>(static_cast<RValueTuple>(tuple)
                               ) };
}

template <typename T>
core::renderer::resources::RandomAccessBuffer<T>::RandomAccessBuffer(
    base::Buffer&&     buffer,
    base::Allocation&& allocation
)
    : m_buffer{ std::move(buffer) },
      m_allocation{ std::move(allocation) }
{}
