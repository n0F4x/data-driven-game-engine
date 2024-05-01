#pragma once

#include <span>
#include <string>

#include <gsl/pointers>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <VkBootstrap.h>

#include "core/renderer/memory/Buffer.hpp"
#include "core/renderer/memory/MappedBuffer.hpp"
#include "core/renderer/wrappers/vma/Allocator.hpp"

namespace core::renderer {

class Instance;
class Device;

class Allocator {
public:
    class Requirements;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Allocator(const Instance& t_instance, const Device& t_device);

    ///-------------///
    ///  Operators  ///
    ///-------------///
    [[nodiscard]]
    auto operator*() const noexcept -> VmaAllocator;
    [[nodiscard]]
    auto operator->() const noexcept -> const VmaAllocator*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto get() const noexcept -> VmaAllocator;

    [[nodiscard]]
    auto create_buffer(
        const vk::BufferCreateInfo&    t_buffer_create_info,
        const VmaAllocationCreateInfo& t_allocation_create_info = {
            .usage = VMA_MEMORY_USAGE_AUTO,
        },
        VkDeviceSize             t_min_alignment = 8
    ) const -> Buffer;

    [[nodiscard]]
    auto create_mapped_buffer(
        const vk::BufferCreateInfo& t_buffer_create_info,
        VkDeviceSize                t_min_alignment = 8
    ) const -> MappedBuffer;

    [[nodiscard]]
    auto create_mapped_buffer(
        const vk::BufferCreateInfo& t_buffer_create_info,
        gsl::not_null<const void*>  t_data,
        VkDeviceSize                t_min_alignment = 8
    ) const -> MappedBuffer;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vma::Allocator m_allocator;
};

}   // namespace core::renderer
