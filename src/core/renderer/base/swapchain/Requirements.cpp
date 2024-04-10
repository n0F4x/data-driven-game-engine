#include "Requirements.hpp"

#include <GLFW/glfw3.h>

namespace core::renderer {

[[nodiscard]] static auto
    required_instance_extension_names() noexcept -> std::span<const std::string>
{
    static const std::vector s_extensions_names{ []() -> std::vector<std::string> {
        if (!glfwInit()) {
            return {};
        }

        uint32_t             count;
        const char**         glfw_extension_names{ glfwGetRequiredInstanceExtensions(&count) };
        if (glfw_extension_names == nullptr) {
            return {};
        }

        std::vector<std::string> result{};
        result.reserve(count);

        for (uint32_t i{}; i < count; i++) {
            result.emplace_back(glfw_extension_names[i]);
        }

        return result;
    }() };

    return s_extensions_names;
}

auto Swapchain::Requirements::required_instance_settings_are_available(
    const vkb::SystemInfo& t_system_info
) -> bool
{
    return std::ranges::all_of(
        required_instance_extension_names(),
        [&t_system_info](const auto& extension_name) {
            return t_system_info.is_extension_available(extension_name.c_str());
        }
    );
}

auto Swapchain::Requirements::enable_instance_settings(
    const vkb::SystemInfo&,
    vkb::InstanceBuilder& t_builder
) -> void
{
    for (const auto& extension_name : required_instance_extension_names()) {
        t_builder.enable_extension(extension_name.c_str());
    }
}

auto Swapchain::Requirements::require_device_settings(
    vkb::PhysicalDeviceSelector& t_physical_device_selector
) -> void
{
    t_physical_device_selector.add_required_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

auto Swapchain::Requirements::enable_optional_device_settings(vkb::PhysicalDevice&) -> void
{}

}   // namespace core::renderer
