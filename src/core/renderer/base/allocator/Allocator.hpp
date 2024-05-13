#pragma once

#include <optional>
#include <span>
#include <string>

#include <gsl/pointers>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <VkBootstrap.h>

#include "core/renderer/memory/Buffer.hpp"
#include "core/renderer/memory/Image.hpp"
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
        }
    ) const -> Buffer;

    [[nodiscard]]
    auto create_buffer_with_alignment(
         const vk::BufferCreateInfo&    t_buffer_create_info,
         vk::DeviceSize                 t_min_alignment,
         const VmaAllocationCreateInfo& t_allocation_create_info = {
            .usage = VMA_MEMORY_USAGE_AUTO,
         }
    ) const -> Buffer;

    [[nodiscard]]
    auto create_mapped_buffer(const vk::BufferCreateInfo& t_buffer_create_info
    ) const -> MappedBuffer;

    [[nodiscard]]
    auto create_mapped_buffer_with_alignment(
        const vk::BufferCreateInfo& t_buffer_create_info,
        vk::DeviceSize              t_min_alignment
    ) const -> MappedBuffer;

    [[nodiscard]]
    auto create_mapped_buffer(
        const vk::BufferCreateInfo& t_buffer_create_info,
        gsl::not_null<const void*>  t_data
    ) const -> MappedBuffer;

    [[nodiscard]]
    auto create_mapped_buffer_with_alignment(
        const vk::BufferCreateInfo& t_buffer_create_info,
        vk::DeviceSize              t_min_alignment,
        gsl::not_null<const void*>  t_data
    ) const -> MappedBuffer;

    [[nodiscard]]
    auto create_image(
        const vk::ImageCreateInfo&     t_image_create_info,
        const VmaAllocationCreateInfo& t_allocation_create_info
    ) const -> Image;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vma::Allocator m_allocator;
};

}   // namespace core::renderer
