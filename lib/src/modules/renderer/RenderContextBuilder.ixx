module;

#include <cstdint>
#include <expected>
#include <optional>
#include <utility>

#include "utility/lifetime_bound.hpp"

export module ddge.modules.renderer.RenderContextBuilder;

import vulkan_hpp;

import ddge.modules.renderer.RenderContext;
import ddge.modules.vulkan.DeviceBuilder;
import ddge.modules.vulkan.InstanceBuilder;
import ddge.utility.containers.StringLiteral;
import ddge.utility.meta.type_traits.forward_like;

namespace ddge::renderer {

export class RenderContextBuilder {
public:
    struct CreateInfo {
        std::optional<util::StringLiteral> application_name;
        std::optional<uint32_t>            application_version{};
    };

    enum struct ConstructorFailure {
        eVulkanVersionNotSupported,
    };

    enum struct BuildFailure {
        eNoSupportedDeviceFound,
    };

    explicit RenderContextBuilder(
        const CreateInfo&                           create_info,
        [[lifetime_bound]] const vk::raii::Context& context
    );

    template <typename Self_T>
    [[nodiscard]]
    auto instance_builder(this Self_T&& self)
        -> util::meta::forward_like_t<vulkan::InstanceBuilder, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto device_builder(this Self_T&& self)
        -> util::meta::forward_like_t<vulkan::DeviceBuilder, Self_T>;

    [[nodiscard]]
    auto request_default_debug_messenger() -> bool;

    [[nodiscard]]
    auto build() && -> std::expected<RenderContext, BuildFailure>;

private:
    vulkan::InstanceBuilder m_instance_builder;
    bool                    m_request_default_debug_messenger{};
    vulkan::DeviceBuilder   m_device_builder;
};

}   // namespace ddge::renderer

namespace ddge::renderer {

template <typename Self_T>
auto RenderContextBuilder::instance_builder(this Self_T&& self)
    -> util::meta::forward_like_t<vulkan::InstanceBuilder, Self_T>
{
    return std::forward_like<Self_T>(self.m_instance_builder);
}

template <typename Self_T>
auto RenderContextBuilder::device_builder(this Self_T&& self)
    -> util::meta::forward_like_t<vulkan::DeviceBuilder, Self_T>
{
    return std::forward_like<Self_T>(self.m_device_builder);
}

}   // namespace ddge::renderer
