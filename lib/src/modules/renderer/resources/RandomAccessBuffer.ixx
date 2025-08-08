module;

#include <cassert>
#include <cstddef>
#include <span>
#include <tuple>

#include <vk_mem_alloc.h>

#include "utility/lifetime_bound.hpp"

export module modules.renderer.resources.RandomAccessBuffer;

import vulkan_hpp;

import modules.renderer.base.allocator.Allocator;
import modules.renderer.base.resources.Allocation;
import modules.renderer.base.resources.Buffer;
import modules.renderer.base.resources.copy_operations;

namespace modules::renderer::resources {

export template <typename T = std::byte>
class RandomAccessBuffer {
public:
    explicit RandomAccessBuffer(
        const base::Allocator&      allocator,
        const vk::BufferCreateInfo& buffer_create_info,
        const void*                 data = nullptr
    );

    [[nodiscard]]
    auto buffer() const noexcept [[lifetime_bound]] -> vk::Buffer;

    template <std::size_t E>
    auto set(std::span<const T, E> data, std::size_t offset = 0) const -> void;
    auto set(const T& data, std::size_t offset = 0) const -> void;

    template <std::size_t E>
    auto get(std::span<T, E> data, std::size_t offset = 0) const -> void;
    auto get(T& data, std::size_t offset = 0) const -> void;

    [[nodiscard]]
    auto size() const noexcept -> std::size_t;
    [[nodiscard]]
    auto size_bytes() const noexcept -> std::size_t;
    [[nodiscard]]
    auto empty() const noexcept -> bool;

private:
    base::Buffer     m_buffer;
    base::Allocation m_allocation;

    [[nodiscard]]
    static auto make_from(
        std::tuple<base::Buffer, base::Allocation, VmaAllocationInfo>&& tuple,
        const void*                                                     data
    ) noexcept -> RandomAccessBuffer;

    RandomAccessBuffer(base::Buffer&& buffer, base::Allocation&& allocation);
};

}   // namespace modules::renderer::resources

template <typename T>
modules::renderer::resources::RandomAccessBuffer<T>::RandomAccessBuffer(
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
auto modules::renderer::resources::RandomAccessBuffer<T>::buffer() const noexcept
    -> vk::Buffer
{
    return m_buffer.get();
}

template <typename T>
template <std::size_t E>
auto modules::renderer::resources::RandomAccessBuffer<T>::set(
    std::span<const T, E> data,
    const std::size_t     offset
) const -> void
{
    assert(data.size() <= size() - offset);
    base::copy(
        data.data(),
        base::CopyRegion{ .allocation = m_allocation, .offset = offset * sizeof(T) },
        data.size_bytes()
    );
}

template <typename T>
auto modules::renderer::resources::RandomAccessBuffer<T>::set(
    const T&          data,
    const std::size_t offset
) const -> void
{
    assert(!empty());
    base::copy(
        std::addressof(data),
        base::CopyRegion{ .allocation = m_allocation, .offset = offset * sizeof(T) },
        sizeof(T)
    );
}

template <typename T>
template <std::size_t E>
auto modules::renderer::resources::RandomAccessBuffer<T>::get(
    std::span<T, E>   data,
    const std::size_t offset
) const -> void
{
    assert(data.size() <= size() - offset);
    base::copy(
        base::CopyRegion{ .allocation = m_allocation, .offset = offset * sizeof(T) },
        data.data(),
        data.size_bytes()
    );
}

template <typename T>
auto modules::renderer::resources::RandomAccessBuffer<T>::get(
    T&                data,
    const std::size_t offset
) const -> void
{
    assert(!empty());
    base::copy(
        base::CopyRegion{ .allocation = m_allocation, .offset = offset * sizeof(T) },
        std::addressof(data),
        sizeof(T)
    );
}

template <typename T>
auto modules::renderer::resources::RandomAccessBuffer<T>::size() const noexcept
    -> std::size_t
{
    return size_bytes() / sizeof(T);
}

template <typename T>
auto modules::renderer::resources::RandomAccessBuffer<T>::size_bytes() const noexcept
    -> std::size_t
{
    return m_buffer.size();
}

template <typename T>
auto modules::renderer::resources::RandomAccessBuffer<T>::empty() const noexcept -> bool
{
    return m_buffer.size() == 0;
}

template <typename T>
auto modules::renderer::resources::RandomAccessBuffer<T>::make_from(
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
modules::renderer::resources::RandomAccessBuffer<T>::RandomAccessBuffer(
    base::Buffer&&     buffer,
    base::Allocation&& allocation
)
    : m_buffer{ std::move(buffer) },
      m_allocation{ std::move(allocation) }
{}
