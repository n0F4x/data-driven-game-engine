#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include "core/utility/lifetime_bound.hpp"

#include "MemoryTypeIndex.hpp"
#include "MemoryView.hpp"

namespace core::renderer::base {

class Allocator;

class Allocation {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
     Allocation()                  = default;
     Allocation(const Allocation&) = delete;
     Allocation(Allocation&&) noexcept;
    ~Allocation() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(const Allocation&) -> Allocation& = delete;
    auto operator=(Allocation&&) noexcept -> Allocation&;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto reset() noexcept -> void;

    [[nodiscard]]
    auto memory_type_index() const noexcept -> MemoryTypeIndex;

    [[nodiscard]]
    auto memory_view() const noexcept [[lifetime_bound]] -> MemoryView;

    [[nodiscard]]
    auto memory_properties() const noexcept -> vk::MemoryPropertyFlags;

    [[nodiscard]]
    auto map() const noexcept -> std::span<std::byte>;
    auto unmap() const noexcept -> void;

    auto flush(vk::DeviceSize offset, vk::DeviceSize size) const -> void;

protected:
    [[nodiscard]]
    auto allocation() const noexcept -> VmaAllocation;
    [[nodiscard]]
    auto allocator() const noexcept -> VmaAllocator;

private:
    friend Allocator;

    VmaAllocator  m_allocator{};
    VmaAllocation m_allocation{};

    MemoryTypeIndex m_memory_type_index{ invalid_memory_type_index_v };
    vk::DeviceSize  m_size{};

    explicit Allocation(
        VmaAllocator   allocator,
        VmaAllocation  allocation,
        uint32_t       memory_type_index,
        vk::DeviceSize size
    ) noexcept;
};

}   // namespace core::renderer::base
