#pragma once

#include <expected>
#include <utility>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include "engine/utility/vma/Allocator.hpp"
#include "engine/utility/vma/Buffer.hpp"

namespace engine::renderer {

class Allocator {
public:
    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        const VmaAllocatorCreateInfo& t_vma_allocator_create_info
    ) noexcept -> std::expected<Allocator, vk::Result>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Allocator(vma::Allocator&& t_allocator) noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    [[nodiscard]] auto operator*() const noexcept -> VmaAllocator;
    [[nodiscard]] auto operator->() const noexcept -> const VmaAllocator*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto create_buffer(
        const vk::BufferCreateInfo&    t_buffer_create_info,
        const VmaAllocationCreateInfo& t_allocation_create_info,
        const void*                    t_data = nullptr
    ) const noexcept -> tl::optional<std::pair<vma::Buffer, VmaAllocationInfo>>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vma::Allocator m_allocator;
};

}   // namespace engine::renderer
