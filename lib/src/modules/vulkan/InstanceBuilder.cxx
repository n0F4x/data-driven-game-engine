module;

#include <algorithm>
#include <cstring>
#include <vector>

#include "utility/contract_macros.hpp"

module ddge.modules.vulkan.InstanceBuilder;

import vulkan_hpp;

import ddge.modules.vulkan.result.check_result;
import ddge.utility.contracts;

namespace ddge::vulkan {

auto InstanceBuilderPrecondition::check_vulkan_version_support(
    const vk::raii::Context& context
) -> bool
{
    return context.getDispatcher()->vkEnumerateInstanceVersion != nullptr
        && context.enumerateInstanceVersion() >= minimum_vulkan_api_version();
}

InstanceBuilderPrecondition::InstanceBuilderPrecondition(const vk::raii::Context& context)
{
    PRECOND(check_vulkan_version_support(context));
}

auto InstanceBuilder::check_vulkan_version_support(const vk::raii::Context& context)
    -> bool
{
    return InstanceBuilderPrecondition::check_vulkan_version_support(context);
}

InstanceBuilder::InstanceBuilder(
    const CreateInfo&        create_info,
    const vk::raii::Context& context
)
    : InstanceBuilderPrecondition{ context },
      m_context{ context },
      m_application_name{ create_info.application_name },
      m_application_version{ create_info.application_version },
      m_engine_name{ create_info.engine_name },
      m_engine_version{ create_info.engine_version }
{}

auto InstanceBuilder::request_vulkan_api_version(const uint32_t vulkan_api_version)
    -> void
{
    if (vulkan_api_version > m_vulkan_api_version) {
        m_vulkan_api_version = vulkan_api_version;
    }
}

auto InstanceBuilder::require_minimum_vulkan_api_version(const uint32_t vulkan_api_version)
    -> bool
{
    if (m_vulkan_api_version >= vulkan_api_version) {
        return true;
    }

    if (vulkan_api_version > m_context.get().enumerateInstanceVersion()) {
        return false;
    }

    m_vulkan_api_version = vulkan_api_version;
    return true;
}

auto InstanceBuilder::enable_vulkan_layer(const char* layer_name) -> bool
{
    const std::vector<vk::LayerProperties> layer_properties{
        m_context.get().enumerateInstanceLayerProperties()
    };

    if (std::ranges::none_of(
            layer_properties,
            [layer_name](const char* const present_layer_name) -> bool {
                return std::strcmp(layer_name, present_layer_name) == 0;
            },
            &vk::LayerProperties::layerName
        ))
    {
        return false;
    }

    if (std::ranges::none_of(
            m_layer_names,
            [layer_name](const char* const enabled_layer_name) -> bool {
                return std::strcmp(layer_name, enabled_layer_name) == 0;
            }
        ))
    {
        m_layer_names.push_back(layer_name);
    }

    return true;
}

auto InstanceBuilder::enable_extension(const char* extension_name) -> bool
{
    const std::vector<vk::ExtensionProperties> extension_properties{
        m_context.get().enumerateInstanceExtensionProperties()
    };

    if (std::ranges::none_of(
            extension_properties,
            [extension_name](const char* const supported_extension) -> bool {
                return std::strcmp(extension_name, supported_extension) == 0;
            },
            &vk::ExtensionProperties::extensionName
        ))
    {
        return false;
    }

    if (std::ranges::none_of(
            m_extension_names,
            [extension_name](const char* const enabled_extension) -> bool {
                return std::strcmp(extension_name, enabled_extension) == 0;
            }
        ))
    {
        m_extension_names.push_back(extension_name);
    }

    return true;
}

auto InstanceBuilder::build() && -> vk::raii::Instance
{
    const vk::ApplicationInfo application_info{
        .pApplicationName   = m_application_name,
        .applicationVersion = m_application_version,
        .pEngineName        = m_engine_name,
        .engineVersion      = m_engine_version,
        .apiVersion         = m_vulkan_api_version,
    };

    const vk::InstanceCreateInfo instance_create_info{
        .pApplicationInfo        = &application_info,
        .enabledLayerCount       = static_cast<uint32_t>(m_layer_names.size()),
        .ppEnabledLayerNames     = m_layer_names.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(m_extension_names.size()),
        .ppEnabledExtensionNames = m_extension_names.data(),
    };

    return vulkan::check_result(m_context.get().createInstance(instance_create_info));
}

}   // namespace ddge::vulkan
