#pragma once

#include <gsl-lite/gsl-lite.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace core::renderer::base {

class Instance;
class Device;

class Allocation;
class Buffer;
class Image;

class Allocator {
public:
    class Requirements;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Allocator(const Instance& instance, const Device& device);

    [[nodiscard]]
    auto allocate(
        const vk::MemoryRequirements& requirements,
        const VmaAllocationCreateInfo& allocation_create_info = {
            .usage = VMA_MEMORY_USAGE_AUTO,
        }
    ) const -> std::tuple<Allocation, VmaAllocationInfo>;

    [[nodiscard]]
    auto create_buffer(
        const vk::BufferCreateInfo&    buffer_create_info,
        const VmaAllocationCreateInfo& allocation_create_info
    ) const -> std::tuple<Buffer, Allocation, VmaAllocationInfo>;

    [[nodiscard]]
    auto create_image(
        const vk::ImageCreateInfo&     image_create_info,
        const VmaAllocationCreateInfo& allocation_create_info
    ) const -> std::tuple<Image, Allocation, VmaAllocationInfo>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    gsl_lite::not_null<std::unique_ptr<VmaAllocator_T, decltype(&vmaDestroyAllocator)>>
        m_allocator;

    [[nodiscard]]
    auto device() const -> vk::Device;
};

}   // namespace core::renderer::base
