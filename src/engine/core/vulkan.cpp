#include "vulkan.hpp"

#include <vulkan/vulkan_raii.hpp>

namespace engine::utils {

void check_validation_layer_support(
    std::span<const char* const> t_validation_layers
)
{
    auto available_layers{ vk::enumerateInstanceLayerProperties() };

    std::vector<const char*> invalid_layers;

    for (auto validationLayer : t_validation_layers) {
        if (std::ranges::find_if(
                available_layers,
                [validationLayer](auto availableLayer) {
                    return strcmp(validationLayer, availableLayer) == 0;
                },
                &vk::LayerProperties::layerName
            )
            == available_layers.end())
        {
            invalid_layers.push_back(validationLayer);
        }
    }

    if (!invalid_layers.empty()) {
        std::string message{
            "The following Vulkan validation layers are not "
            "supported:\n"
        };
        for (auto layer : invalid_layers) {
            message += "\t\"";
            message += layer;
            message += "\"\n";
        }
        throw std::runtime_error{ message };
    }
}

void check_extension_support(std::span<const char* const> t_extensions)
{
    auto available_extensions{ vk::enumerateInstanceExtensionProperties() };

    std::vector<const char*> invalid_extensions;

    for (auto extension : t_extensions) {
        if (std::ranges::find_if(
                available_extensions,
                [extension](auto availableLayer) {
                    return strcmp(extension, availableLayer) == 0;
                },
                &vk::ExtensionProperties::extensionName
            )
            == available_extensions.end())
        {
            invalid_extensions.push_back(extension);
        }
    }

    if (!invalid_extensions.empty()) {
        std::string message{
            "The following Vulkan validation layers are not "
            "supported:\n"
        };
        for (auto extension : invalid_extensions) {
            message += "\t\"";
            message += extension;
            message += "\"\n";
        }
        throw std::runtime_error{ message };
    }
}

auto create_instance(
    const vk::ApplicationInfo&   t_appInfo,
    std::span<const char* const> t_validationLayers,
    std::span<const char* const> t_extensions
) -> vk::raii::Instance
{
    check_validation_layer_support(t_validationLayers);
    check_extension_support(t_extensions);
    return {
        {},
        { .pApplicationInfo  = &t_appInfo,
         .enabledLayerCount = static_cast<uint32_t>(t_validationLayers.size()),
         .ppEnabledLayerNames     = t_validationLayers.data(),
         .enabledExtensionCount   = static_cast<uint32_t>(t_extensions.size()),
         .ppEnabledExtensionNames = t_extensions.data() }
    };
}

}   // namespace engine::utils
