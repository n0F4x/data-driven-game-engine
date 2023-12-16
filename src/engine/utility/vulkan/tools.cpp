#include "tools.hpp"

#include <fstream>
#include <ranges>
#include <set>

namespace engine::vulkan {

auto available_layers() noexcept
    -> std::expected<std::vector<std::string>, vk::Result>
{
    const auto [result, properties]{ vk::enumerateInstanceLayerProperties() };

    if (result != vk::Result::eSuccess) {
        return std::unexpected{ result };
    }

    return properties
         | std::views::transform([](const vk::LayerProperties& t_property) {
               return t_property.layerName.operator std::string();
           })
         | std::ranges::to<std::vector<std::string>>();
}

auto available_instance_extensions() noexcept
    -> std::expected<std::vector<std::string>, vk::Result>
{
    const auto [result, properties]{ vk::enumerateInstanceExtensionProperties(
    ) };

    if (result != vk::Result::eSuccess) {
        return std::unexpected{ result };
    }

    return properties | std::views::transform([](const auto& t_property) {
               return t_property.extensionName.operator std::string();
           })
         | std::ranges::to<std::vector>();
}

auto available_device_extensions(vk::PhysicalDevice t_physical_device) noexcept
    -> std::expected<std::vector<std::string>, vk::Result>
{
    const auto [result, extension_properties]{
        t_physical_device.enumerateDeviceExtensionProperties()
    };

    if (result != vk::Result::eSuccess) {
        return std::unexpected{ result };
    }

    return extension_properties
         | std::views::transform([](const auto& t_property) {
               return t_property.extensionName.operator std::string();
           })
         | std::ranges::to<std::vector<std::string>>();
}

auto supports_extensions(
    vk::PhysicalDevice           t_physical_device,
    std::span<const std::string> t_extensions
) noexcept -> bool
{
    if (!t_physical_device) {
        return false;
    }

    const auto [result, extension_properties]{
        t_physical_device.enumerateDeviceExtensionProperties()
    };
    if (result != vk::Result::eSuccess) {
        return false;
    }

    std::set<std::string_view> required_extensions{ t_extensions.begin(),
                                                    t_extensions.end() };

    for (const auto& extension : extension_properties) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

auto supports_surface(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> bool
{
    if (!t_physical_device || !t_surface) {
        return false;
    }

    uint32_t index{};
    for (const auto& properties : t_physical_device.getQueueFamilyProperties())
    {
        auto [result, supported]{
            t_physical_device.getSurfaceSupportKHR(index, t_surface)
        };
        if (result != vk::Result::eSuccess) {
            return false;
        }
        if (properties.queueCount > 0 && supported) {
            return true;
        }
        index++;
    }
    return false;
}

auto load_shader(vk::Device t_device, const std::string& t_file_path) noexcept
    -> vk::UniqueShaderModule
{
    std::ifstream file{ t_file_path,
                        std::ios::binary | std::ios::in | std::ios::ate };
    if (!file.is_open()) {
        return vk::UniqueShaderModule{};
    }

    const std::streamsize file_size = file.tellg();
    if (file_size <= 0) {
        return vk::UniqueShaderModule{};
    }

    std::vector<char> buffer(static_cast<size_t>(file_size));

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), file_size);
    file.close();

    const vk::ShaderModuleCreateInfo create_info{
        .codeSize = static_cast<size_t>(file_size),
        .pCode    = (uint32_t*)buffer.data()
    };

    auto [result, shader_module]{ t_device.createShaderModuleUnique(create_info
    ) };
    if (result != vk::Result::eSuccess) {
        return vk::UniqueShaderModule{};
    }

    return std::move(shader_module);
}

}   // namespace engine::vulkan
