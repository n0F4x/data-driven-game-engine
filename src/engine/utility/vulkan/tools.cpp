#include "tools.hpp"

#include <fstream>
#include <ranges>
#include <set>

namespace engine::utils::vulkan {

auto available_layers() noexcept
    -> std::expected<std::vector<const char*>, vk::Result>
{
    const auto [result, properties]{ vk::enumerateInstanceLayerProperties() };

    if (result != vk::Result::eSuccess) {
        return std::unexpected{ result };
    }

    const auto view{
        properties
        | std::views::transform([](const vk::LayerProperties& t_property) {
              return t_property.layerName;
          })
    };
    return std::vector<const char*>{ view.begin(), view.end() };
}

auto available_instance_extensions() noexcept
    -> std::expected<std::vector<const char*>, vk::Result>
{
    const auto [result, properties]{ vk::enumerateInstanceExtensionProperties(
    ) };

    if (result != vk::Result::eSuccess) {
        return std::unexpected{ result };
    }

    const auto view{
        properties
        | std::views::transform([](const vk::ExtensionProperties& t_property) {
              return t_property.extensionName;
          })
    };
    return std::vector<const char*>{ view.begin(), view.end() };
}

auto available_device_extensions(vk::PhysicalDevice t_physical_device) noexcept
    -> std::expected<std::vector<const char*>, vk::Result>
{
    const auto [result, extension_properties]{
        t_physical_device.enumerateDeviceExtensionProperties()
    };

    if (result != vk::Result::eSuccess) {
        return std::unexpected{ result };
    }

    const auto view{
        extension_properties
        | std::views::transform([](const vk::ExtensionProperties& t_property) {
              return t_property.extensionName;
          })
    };
    return std::vector<const char*>{ view.begin(), view.end() };
}

auto supports_extensions(
    vk::PhysicalDevice           t_physical_device,
    std::span<const char* const> t_extensions
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

    std::set<std::string_view> required_extensions{ t_extensions.cbegin(),
                                                    t_extensions.cend() };

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

    for (const auto [index, properties] :
         std::views::enumerate(t_physical_device.getQueueFamilyProperties()))
    {
        auto [result, supported]{ t_physical_device.getSurfaceSupportKHR(
            static_cast<uint32_t>(index), t_surface
        ) };
        if (result != vk::Result::eSuccess) {
            return false;
        }
        if (properties.queueCount > 0 && supported) {
            return true;
        }
    }
    return false;
}

auto load_shader(vk::Device t_device, std::string_view t_file_path) noexcept
    -> tl::optional<vk::ShaderModule>
{
    std::ifstream file{ t_file_path.data(),
                        std::ios::binary | std::ios::in | std::ios::ate };
    if (!file.is_open()) {
        return tl::nullopt;
    }

    const std::streamsize file_size = file.tellg();
    if (file_size <= 0) {
        return tl::nullopt;
    }

    std::vector<char> buffer(static_cast<size_t>(file_size));

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), file_size);
    file.close();

    const vk::ShaderModuleCreateInfo create_info{
        .codeSize = static_cast<size_t>(file_size),
        .pCode    = (uint32_t*)buffer.data()
    };

    const auto [result, shader_module]{ t_device.createShaderModule(create_info
    ) };
    if (result != vk::Result::eSuccess) {
        return tl::nullopt;
    }

    return shader_module;
}

}   // namespace engine::utils::vulkan
