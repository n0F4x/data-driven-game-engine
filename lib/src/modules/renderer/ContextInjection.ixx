module;

#include <functional>
#include <utility>

export module ddge.modules.renderer.ContextInjection;

import vulkan_hpp;

import ddge.modules.renderer.Context;
import ddge.modules.vulkan.default_debug_messenger_callback;
import ddge.modules.vulkan.DeviceBuilder;
import ddge.modules.vulkan.InstanceBuilder;
import ddge.modules.wsi.Context;
import ddge.util.containers.OptionalRef;
import ddge.util.meta.type_traits.forward_like;

namespace ddge::renderer {

export class ContextInjection {
public:
    enum struct BuildFailure {
        eNoSupportedDeviceFound,
    };

    explicit ContextInjection(vulkan::InstanceBuilder& instance_builder);

    [[nodiscard]]
    auto instance_builder() const -> vulkan::InstanceBuilder&;

    auto request_default_debug_messenger() -> bool;
    auto request_wsi_support() -> void;

    template <typename Self_T>
    [[nodiscard]]
    auto device_builder(this Self_T&& self)
        -> util::meta::forward_like_t<vulkan::DeviceBuilder, Self_T>;

    [[nodiscard]]
    auto operator()(
        const vk::raii::Instance&             instance,
        util::OptionalRef<const wsi::Context> wsi_context
    ) && -> Context;

private:
    std::reference_wrapper<vulkan::InstanceBuilder> m_instance_builder;
    bool                                            m_request_default_debug_messenger{};
    bool                                            m_request_wsi_support{};
    vulkan::DeviceBuilder                           m_device_builder;
};

}   // namespace ddge::renderer

namespace ddge::renderer {

template <typename Self_T>
auto ContextInjection::device_builder(this Self_T&& self)
    -> util::meta::forward_like_t<vulkan::DeviceBuilder, Self_T>
{
    return std::forward_like<Self_T>(self.m_device_builder);
}

}   // namespace ddge::renderer
