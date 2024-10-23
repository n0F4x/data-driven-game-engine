#pragma once

#include "core/renderer/base/resources/Allocation.hpp"
#include "core/renderer/base/resources/Buffer.hpp"

namespace core::renderer::resources {

template <typename T>
class RandomAccessBuffer {
public:
             RandomAccessBuffer() = default;
    explicit RandomAccessBuffer(
        const base::Allocator&      allocator,
        const vk::BufferCreateInfo& buffer_create_info,
        const void*                 data = nullptr
    );

    [[nodiscard]]
    auto get() const noexcept [[lifetime_bound]] -> vk::Buffer;

    template <size_t E>
    auto set(std::span<const T, E> data, size_t offset = 0) const -> void;
    auto set(const T& data, size_t offset = 0) const -> void;

    [[nodiscard]]
    auto size() const noexcept -> size_t;
    [[nodiscard]]
    auto size_bytes() const noexcept -> size_t;

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

}   // namespace core::renderer::resources

#include "RandomAccessBuffer.inl"
