#pragma once

#include <expected>
#include <span>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace engine::renderer::helpers {

[[nodiscard]] auto application_info() noexcept -> const vk::ApplicationInfo&;

[[nodiscard]] auto layers() noexcept -> std::span<const char* const>;

[[nodiscard]] auto instance_extensions() noexcept
    -> std::span<const char* const>;

[[nodiscard]] auto create_debug_messenger(vk::Instance t_instance) noexcept
    -> std::expected<vk::DebugUtilsMessengerEXT, vk::Result>;

[[nodiscard]] auto device_extensions(vk::PhysicalDevice t_physical_device
) noexcept -> std::span<const char* const>;

[[nodiscard]] auto is_adequate(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> bool;

[[nodiscard]] auto choose_physical_device(
    vk::Instance   t_instance,
    vk::SurfaceKHR t_surface
) noexcept -> vk::PhysicalDevice;

[[nodiscard]] auto vma_allocator_create_flags(
    std::span<const char* const> enabled_instance_extensions,
    std::span<const char* const> enabled_device_extensions
) noexcept -> VmaAllocatorCreateFlags;

}   // namespace engine::renderer::helpers
