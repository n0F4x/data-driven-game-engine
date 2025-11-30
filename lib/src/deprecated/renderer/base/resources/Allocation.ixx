module;

#include <span>

#include <vk_mem_alloc.h>

#include "utility/lifetime_bound.hpp"

export module ddge.deprecated.renderer.base.resources.Allocation;

import vulkan_hpp;

import ddge.deprecated.renderer.base.resources.MemoryTypeIndex;
import ddge.deprecated.renderer.base.resources.MemoryView;

namespace ddge::renderer::base {

export class Allocation {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    // TODO: allow only Allocator to construct this
    explicit Allocation(
        VmaAllocator   allocator,
        VmaAllocation  allocation,
        uint32_t       memory_type_index,
        vk::DeviceSize size
    ) noexcept;
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

    auto invalidate(vk::DeviceSize offset, vk::DeviceSize size) const -> void;
    auto flush(vk::DeviceSize offset, vk::DeviceSize size) const -> void;

protected:
    [[nodiscard]]
    auto allocation() const noexcept -> VmaAllocation;
    [[nodiscard]]
    auto allocator() const noexcept -> VmaAllocator;

private:
    VmaAllocator  m_allocator{};
    VmaAllocation m_allocation{};

    MemoryTypeIndex m_memory_type_index{ invalid_memory_type_index_v };
    vk::DeviceSize  m_size{};
};

}   // namespace ddge::renderer::base
