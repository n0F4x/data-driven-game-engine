#pragma once

#include <expected>
#include <span>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace engine::renderer::helpers {

[[nodiscard]] auto application_info() noexcept -> const vk::ApplicationInfo&;

[[nodiscard]] auto layers() noexcept -> std::span<const std::string>;

[[nodiscard]] auto instance_extensions() noexcept
    -> std::span<const std::string>;

[[nodiscard]] auto create_debug_messenger(vk::Instance t_instance)
    -> vk::UniqueDebugUtilsMessengerEXT;

struct QueueInfos {
    uint32_t                               graphics_family;
    uint32_t                               graphics_index;
    uint32_t                               compute_family;
    uint32_t                               compute_index;
    uint32_t                               transfer_family;
    uint32_t                               transfer_index;
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
};

[[nodiscard]] auto find_queue_families(
    vk::PhysicalDevice queue_family_index,
    vk::SurfaceKHR     t_surface
) -> tl::optional<QueueInfos>;

[[nodiscard]] auto device_extensions(vk::PhysicalDevice t_physical_device
) noexcept -> std::span<const std::string>;

[[nodiscard]] auto is_adequate(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> bool;

[[nodiscard]] auto choose_physical_device(
    vk::Instance   t_instance,
    vk::SurfaceKHR t_surface
) -> vk::PhysicalDevice;

[[nodiscard]] auto vma_allocator_create_flags(
    std::span<const std::string> enabled_instance_extensions,
    std::span<const std::string> enabled_device_extensions
) noexcept -> VmaAllocatorCreateFlags;

}   // namespace engine::renderer::helpers
