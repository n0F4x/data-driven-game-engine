#pragma once

#include <span>
#include <string>

#include <vulkan/vulkan.hpp>

namespace plugins::renderer {

[[nodiscard]] auto application_info() noexcept -> const vk::ApplicationInfo&;

[[nodiscard]] auto layers() noexcept -> std::span<const std::string>;

[[nodiscard]] auto instance_extensions() noexcept -> std::span<const std::string>;

[[nodiscard]] auto filter(
    std::span<const std::string> t_available,
    std::span<const std::string> t_required,
    std::span<const std::string> t_optional
) noexcept -> std::vector<std::string>;

[[nodiscard]] auto create_debug_messenger(vk::Instance t_instance)
    -> vk::UniqueDebugUtilsMessengerEXT;

[[nodiscard]] auto is_adequate(
    vk::PhysicalDevice           t_physical_device,
    vk::SurfaceKHR               t_surface,
    std::span<const std::string> t_required_extension_names
) noexcept -> bool;

[[nodiscard]] auto choose_physical_device(
    vk::Instance                 t_instance,
    vk::SurfaceKHR               t_surface,
    std::span<const std::string> t_required_extension_names
) -> vk::PhysicalDevice;

}   // namespace plugins::renderer
