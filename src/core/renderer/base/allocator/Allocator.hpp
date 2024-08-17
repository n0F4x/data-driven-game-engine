#pragma once

#include <optional>
#include <span>
#include <string>

#include <gsl-lite/gsl-lite.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <VkBootstrap.h>

#include "core/renderer/memory/Buffer.hpp"
#include "core/renderer/memory/Image.hpp"
#include "core/renderer/memory/MappedBuffer.hpp"

namespace core::renderer {

class Instance;
class Device;

class Allocator {
public:
    class Requirements;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Allocator(const Instance& instance, const Device& device);

    ///-------------///
    ///  Operators  ///
    ///-------------///
    [[nodiscard]]
    auto operator*() const noexcept -> const VmaAllocator_T&;
    [[nodiscard]]
    auto operator*() noexcept -> VmaAllocator_T&;
    [[nodiscard]]
    auto operator->() const noexcept -> VmaAllocator;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto get() const noexcept -> VmaAllocator;

    [[nodiscard]]
    auto allocate_buffer(
        const vk::BufferCreateInfo&    buffer_create_info,
        const VmaAllocationCreateInfo& allocation_create_info = {
            .usage = VMA_MEMORY_USAGE_AUTO,
        }
    ) const -> Buffer;

    [[nodiscard]]
    auto allocate_buffer_with_alignment(
         const vk::BufferCreateInfo&    buffer_create_info,
         vk::DeviceSize                 min_alignment,
         const VmaAllocationCreateInfo& allocation_create_info = {
            .usage = VMA_MEMORY_USAGE_AUTO,
         }
    ) const -> Buffer;

    [[nodiscard]]
    auto allocate_mapped_buffer(const vk::BufferCreateInfo& buffer_create_info
    ) const -> MappedBuffer;

    [[nodiscard]]
    auto allocate_mapped_buffer_with_alignment(
        const vk::BufferCreateInfo& buffer_create_info,
        vk::DeviceSize              min_alignment
    ) const -> MappedBuffer;

    [[nodiscard]]
    auto allocate_mapped_buffer(
        const vk::BufferCreateInfo&        buffer_create_info,
        gsl_lite::not_null_ic<const void*> data
    ) const -> MappedBuffer;

    [[nodiscard]]
    auto allocate_mapped_buffer_with_alignment(
        const vk::BufferCreateInfo&        buffer_create_info,
        vk::DeviceSize                     min_alignment,
        gsl_lite::not_null_ic<const void*> data
    ) const -> MappedBuffer;

    [[nodiscard]]
    auto allocate_image(
        const vk::ImageCreateInfo&     image_create_info,
        const VmaAllocationCreateInfo& allocation_create_info
    ) const -> Image;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    gsl_lite::not_null<std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)>>
        m_allocator;
};

}   // namespace core::renderer
