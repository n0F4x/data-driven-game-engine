module;

#include <gsl-lite/gsl-lite.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <VkBootstrap.h>

export module core.renderer.base.allocator.Allocator;

import core.renderer.base.instance.Instance;
import core.renderer.base.device.Device;

import core.renderer.base.resources.Allocation;
import core.renderer.base.resources.Buffer;
import core.renderer.base.resources.Image;

namespace core::renderer::base {

export class Allocator {
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

class Allocator::Requirements {
public:
    [[nodiscard]]
    static auto required_instance_settings_are_available(
        const vkb::SystemInfo& system_info
    ) -> bool;

    static auto enable_instance_settings(
        const vkb::SystemInfo& system_info,
        vkb::InstanceBuilder&  builder
    ) -> void;

    static auto require_device_settings(
        vkb::PhysicalDeviceSelector& physical_device_selector
    ) -> void;

    static auto enable_optional_device_settings(vkb::PhysicalDevice& physical_device)
        -> void;
};

}   // namespace core::renderer::base
