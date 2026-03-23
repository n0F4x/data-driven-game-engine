module;

#include <format>
#include <optional>
#include <utility>

#include "modules/log/log_macros.hpp"
#include "util/contract_macros.hpp"

module ddge.modules.renderer.ContextInjection;

import vulkan_hpp;

import ddge.modules.log;
import ddge.modules.vulkan.QueueFamilyIndex;
import ddge.modules.vulkan.result.check_result;
import ddge.modules.wsi.vulkan_queue_family_supports_presenting;
import ddge.util.contracts;

namespace ddge::renderer {

ContextInjection::ContextInjection(vulkan::InstanceBuilder& instance_builder)
    : m_instance_builder{ instance_builder }
{}

auto ContextInjection::instance_builder() const -> vulkan::InstanceBuilder&
{
    return m_instance_builder;
}

auto ContextInjection::request_default_debug_messenger() -> bool
{
    if (m_request_default_debug_messenger) {
        return true;
    }

    if (!m_instance_builder.get().enable_extension(vk::EXTDebugUtilsExtensionName)) {
        return false;
    }

    m_request_default_debug_messenger = true;

    return true;
}

auto ContextInjection::request_wsi_support() -> void
{
    m_request_wsi_support = true;
}

auto ContextInjection::operator()(
    const vk::raii::Instance&             instance,
    util::OptionalRef<const wsi::Context> wsi_context
) && -> Context
{
    constexpr static vk::DebugUtilsMessengerCreateInfoEXT
        default_debug_messenger_create_info{
            .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                             | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
                         | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                         | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                         | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            .pfnUserCallback = vulkan::default_debug_messenger_callback,
        };

    vk::raii::DebugUtilsMessengerEXT default_debug_messenger{
        m_request_default_debug_messenger
            ? vulkan::check_result(instance.createDebugUtilsMessengerEXT(
                  default_debug_messenger_create_info
              ))
            : vk::raii::DebugUtilsMessengerEXT{ nullptr }
    };

    if (m_request_wsi_support) {
        PRECOND(wsi_context.has_value());

        device_builder().enable_extension(vk::KHRSwapchainExtensionName);
        device_builder().ensure_queue(
            [&wsi_context, &instance](
                const vk::raii::PhysicalDevice& physical_device,
                const vulkan::QueueFamilyIndex  queue_family_index,
                const vk::QueueFamilyProperties2&
            ) -> bool {
                return wsi::vulkan_queue_family_supports_presenting(
                    *wsi_context, instance, physical_device, queue_family_index
                );
            }
        );
    }

    std::optional optional_device{ std::move(m_device_builder).build(instance) };
    if (!optional_device.has_value()) {
        throw BuildFailure::eNoSupportedDeviceFound;
    }

    ENGINE_LOG_INFO(
        std::format(
            "Created renderer for GPU - {}",
            static_cast<const char*>(
                optional_device->physical_device.getProperties2().properties.deviceName
            )
        )
    );

    return Context{
        .instance                = instance,
        .default_debug_messenger = std::move(default_debug_messenger),
        .device                  = *std::move(optional_device),
    };
}

}   // namespace ddge::renderer
