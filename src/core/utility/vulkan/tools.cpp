#include "tools.hpp"

#include <fstream>
#include <ranges>
#include <set>

namespace core::vulkan {

auto available_layers() -> std::vector<std::string>
{
    return vk::enumerateInstanceLayerProperties()
         | std::views::transform([](const vk::LayerProperties& t_property) {
               return t_property.layerName.operator std::string();
           })
         | std::ranges::to<std::vector<std::string>>();
}

auto available_instance_extensions() -> std::vector<std::string>
{
    return vk::enumerateInstanceExtensionProperties()
         | std::views::transform([](const auto& t_property) {
               return t_property.extensionName.operator std::string();
           })
         | std::ranges::to<std::vector>();
}

auto available_device_extensions(const vk::PhysicalDevice t_physical_device)
    -> std::vector<std::string>
{
    return t_physical_device.enumerateDeviceExtensionProperties()
         | std::views::transform([](const auto& t_property) {
               return t_property.extensionName.operator std::string();
           })
         | std::ranges::to<std::vector<std::string>>();
}

auto supports_extensions(
    const vk::PhysicalDevice     t_physical_device,
    std::span<const std::string> t_extensions
) -> bool
{
    assert(t_physical_device);
    if (!t_physical_device) {
        return false;
    }

    const auto extension_properties{ t_physical_device.enumerateDeviceExtensionProperties(
    ) };

    std::set<std::string_view> required_extensions{ std::from_range, t_extensions };

    for (const auto& extension : extension_properties) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

auto supports_surface(
    const vk::PhysicalDevice t_physical_device,
    const vk::SurfaceKHR     t_surface
) noexcept -> bool
{
    assert(t_physical_device);
    assert(t_surface);
    if (!t_physical_device || !t_surface) {
        return false;
    }

    uint32_t index{};
    for (const auto& properties : t_physical_device.getQueueFamilyProperties()) {
        if (properties.queueCount > 0
            && t_physical_device.getSurfaceSupportKHR(index, t_surface))
        {
            return true;
        }
        index++;
    }
    return false;
}

auto load_shader(const vk::Device t_device, const std::filesystem::path& t_filepath)
    -> vk::UniqueShaderModule
{
    std::ifstream file{ t_filepath, std::ios::binary | std::ios::in | std::ios::ate };

    const std::streamsize file_size = file.tellg();
    if (file_size == -1) {
        return {};
    }

    std::vector<char> buffer(static_cast<size_t>(file_size));

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), file_size);
    file.close();

    const vk::ShaderModuleCreateInfo create_info{
        .codeSize = static_cast<size_t>(file_size), .pCode = reinterpret_cast<uint32_t*>(buffer.data())
    };

    return t_device.createShaderModuleUnique(create_info);
}

}   // namespace core::vulkan
