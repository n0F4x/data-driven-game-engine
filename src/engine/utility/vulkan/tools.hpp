#pragma once

#include <expected>
#include <span>
#include <string_view>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

namespace engine::vulkan {

[[nodiscard]] auto available_layers() noexcept
    -> std::expected<std::vector<const char*>, vk::Result>;

[[nodiscard]] auto available_instance_extensions() noexcept
    -> std::expected<std::vector<const char*>, vk::Result>;

[[nodiscard]] auto available_device_extensions(
    vk::PhysicalDevice t_physical_device
) noexcept -> std::expected<std::vector<const char*>, vk::Result>;

[[nodiscard]] auto supports_extensions(
    vk::PhysicalDevice           t_physical_device,
    std::span<const char* const> t_extensions
) noexcept -> bool;

[[nodiscard]] auto supports_surface(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> bool;

[[nodiscard]] auto load_shader(
    vk::Device       t_device,
    std::string_view t_file_path
) noexcept -> tl::optional<vk::ShaderModule>;

}   // namespace engine::vulkan
