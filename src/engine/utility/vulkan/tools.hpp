#pragma once

#include <span>
#include <string>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

[[nodiscard]] auto available_layers() -> std::vector<std::string>;

[[nodiscard]] auto available_instance_extensions() -> std::vector<std::string>;

[[nodiscard]] auto available_device_extensions(
    vk::PhysicalDevice t_physical_device
) -> std::vector<std::string>;

[[nodiscard]] auto supports_extensions(
    vk::PhysicalDevice           t_physical_device,
    std::span<const std::string> t_extensions
) -> bool;

[[nodiscard]] auto supports_surface(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> bool;

[[nodiscard]] auto load_shader(
    vk::Device         t_device,
    const std::string& t_file_path
) -> vk::UniqueShaderModule;

}   // namespace engine::vulkan
