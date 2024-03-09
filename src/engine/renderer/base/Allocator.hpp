#pragma once

#include <expected>
#include <utility>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include "engine/utility/vma/Allocator.hpp"
#include "engine/utility/vma/Buffer.hpp"

namespace engine::renderer {

class Instance;
class Device;

class Allocator {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Allocator(const VmaAllocatorCreateInfo& t_vma_allocator_create_info);
    explicit Allocator(const Instance& t_instance, const Device& t_device);
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
