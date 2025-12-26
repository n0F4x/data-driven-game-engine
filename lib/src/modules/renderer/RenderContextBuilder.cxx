module;

#include <expected>
#include <optional>
#include <utility>

module ddge.modules.renderer.RenderContextBuilder;

import vulkan_hpp;

import ddge.modules.config.engine_name;
import ddge.modules.config.engine_version;
import ddge.modules.vulkan.default_debug_messenger_callback;
import ddge.modules.vulkan.result.check_result;

namespace ddge::renderer {

[[nodiscard]]
constexpr auto make_instance_create_info(
    const RenderContextBuilder::CreateInfo& create_info
) noexcept -> vulkan::InstanceBuilder::CreateInfo
{
    return vulkan::InstanceBuilder::CreateInfo{
        .application_name    = create_info.application_name,
        .application_version = create_info.application_version,
        .engine_name         = config::engine_name(),
        .engine_version      = vk::makeApiVersion(
            0u,
            config::engine_version().major,
            config::engine_version().minor,
            config::engine_version().patch
        ),
    };
}

auto ensure_instance_builder_precondition(const vk::raii::Context& context)
    -> const vk::raii::Context&
{
    if (!vulkan::InstanceBuilder::check_vulkan_version_support(context)) {
        throw RenderContextBuilder::ConstructorFailure::eVulkanVersionNotSupported;
    }
    return context;
}

RenderContextBuilder::RenderContextBuilder(
    const CreateInfo&        create_info,
    const vk::raii::Context& context
)
    : m_instance_builder{ make_instance_create_info(create_info), context }
{}

auto RenderContextBuilder::request_default_debug_messenger() -> bool
{
    if (m_request_default_debug_messenger) {
        return true;
    }

    if (!m_instance_builder.enable_extension(vk::EXTDebugUtilsExtensionName)) {
        return false;
    }

    m_request_default_debug_messenger = true;

    return true;
}

auto ddge::renderer::RenderContextBuilder::build() && -> std::
    expected<RenderContext, BuildFailure>
{
    vk::raii::Instance instance{ std::move(m_instance_builder).build() };

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

    std::optional optional_device{ std::move(m_device_builder).build(instance) };
    if (!optional_device.has_value()) {
        return std::unexpected<BuildFailure>{ BuildFailure::eNoSupportedDeviceFound };
    }

    return RenderContext{
        .instance                = std::move(instance),
        .default_debug_messenger = std::move(default_debug_messenger),
        .device                  = *std::move(optional_device),
    };
}

}   // namespace ddge::renderer
