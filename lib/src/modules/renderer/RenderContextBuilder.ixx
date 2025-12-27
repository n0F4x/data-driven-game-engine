module;

#include <cstdint>
#include <expected>
#include <functional>
#include <optional>
#include <utility>

export module ddge.modules.renderer.RenderContextBuilder;

import vulkan_hpp;

import ddge.modules.renderer.RenderContext;
import ddge.modules.vulkan.default_debug_messenger_callback;
import ddge.modules.vulkan.DeviceBuilder;
import ddge.modules.vulkan.InstanceBuilder;
import ddge.modules.vulkan.result.check_result;
import ddge.utility.containers.StringLiteral;
import ddge.utility.meta.type_traits.forward_like;

namespace ddge::renderer {

export class RenderContextBuilder {
public:
    enum struct BuildFailure {
        eNoSupportedDeviceFound,
    };

    explicit RenderContextBuilder(vulkan::InstanceBuilder& instance_builder);

    [[nodiscard]]
    auto instance_builder() const -> vulkan::InstanceBuilder&;

    [[nodiscard]]
    auto request_default_debug_messenger() -> bool;

    template <typename Self_T>
    [[nodiscard]]
    auto device_builder(this Self_T&& self)
        -> util::meta::forward_like_t<vulkan::DeviceBuilder, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto build(this Self_T&&, const vk::raii::Instance& instance)
        -> std::expected<RenderContext, BuildFailure>;

private:
    std::reference_wrapper<vulkan::InstanceBuilder> m_instance_builder;
    bool                                            m_request_default_debug_messenger{};
    vulkan::DeviceBuilder                           m_device_builder;
};

}   // namespace ddge::renderer

namespace ddge::renderer {

template <typename Self_T>
auto RenderContextBuilder::device_builder(this Self_T&& self)
    -> util::meta::forward_like_t<vulkan::DeviceBuilder, Self_T>
{
    return std::forward_like<Self_T>(self.m_device_builder);
}

template <typename Self_T>
auto RenderContextBuilder::build(this Self_T&& self, const vk::raii::Instance& instance)
    -> std::expected<RenderContext, BuildFailure>
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
        self.m_request_default_debug_messenger
            ? vulkan::check_result(instance.createDebugUtilsMessengerEXT(
                  default_debug_messenger_create_info
              ))
            : vk::raii::DebugUtilsMessengerEXT{ nullptr }
    };

    std::optional optional_device{
        std::forward_like<Self_T>(self.m_device_builder).build(instance)
    };
    if (!optional_device.has_value()) {
        return std::unexpected<BuildFailure>{ BuildFailure::eNoSupportedDeviceFound };
    }

    return RenderContext{
        .instance                = instance,
        .default_debug_messenger = std::move(default_debug_messenger),
        .device                  = *std::move(optional_device),
    };
}

}   // namespace ddge::renderer
