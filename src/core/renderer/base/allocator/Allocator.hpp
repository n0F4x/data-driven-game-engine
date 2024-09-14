#pragma once

#include <gsl-lite/gsl-lite.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace core::renderer {

class Instance;
class Device;

class Buffer;
template <typename T>
class SeqWriteBuffer;
template <typename T>
class RandomAccessBuffer;
class Image;

class Allocator {
public:
    class Requirements;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Allocator(const Instance& instance, const Device& device);

    [[nodiscard]]
    auto allocate_buffer(
        const vk::BufferCreateInfo&    buffer_create_info,
        const VmaAllocationCreateInfo& allocation_create_info = {
            .usage = VMA_MEMORY_USAGE_AUTO,
        }
    ) const -> Buffer;

    template <typename T = std::byte>
    [[nodiscard]]
    auto allocate_seq_write_buffer(
        const vk::BufferCreateInfo& buffer_create_info,
        const void*                 data = nullptr
    ) const -> SeqWriteBuffer<T>;

    template <typename T = std::byte>
    [[nodiscard]]
    auto allocate_random_access_buffer(
        const vk::BufferCreateInfo& buffer_create_info,
        const void*                 data = nullptr
    ) const -> RandomAccessBuffer<T>;

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

#include "Allocator.inl"
