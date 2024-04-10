#pragma once

#include <filesystem>
#include <span>
#include <string>

#include <vulkan/vulkan.hpp>

namespace core::vulkan {

[[nodiscard]] auto filter(
    const std::ranges::range auto& t_available,
    const std::ranges::range auto& t_required,
    const std::ranges::range auto& t_optional
) noexcept -> std::vector<std::string>;

[[nodiscard]] auto available_layers() -> std::vector<std::string>;

[[nodiscard]] auto available_instance_extensions() -> std::vector<std::string>;

[[nodiscard]] auto available_device_extensions(vk::PhysicalDevice t_physical_device)
    -> std::vector<std::string>;

[[nodiscard]] auto supports_extensions(
    vk::PhysicalDevice             t_physical_device,
    const std::ranges::range auto& t_extension_names
) -> bool;

[[nodiscard]] auto supports_surface(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> bool;

template <typename T, typename ExtendedStruct>
concept ExtendsStruct = requires { vk::StructExtends<ExtendedStruct, T>::value; };

[[nodiscard]] auto
    load_shader(vk::Device t_device, const std::filesystem::path& t_filepath)
        -> vk::UniqueShaderModule;

}   // namespace core::vulkan

#include "tools.inl"
