module;

#include <cstdint>
#include <utility>

export module ddge.modules.renderer.RenderContextBuilder;

import vulkan_hpp;

import ddge.modules.renderer.RenderContext;
import ddge.modules.vulkan.InstanceBuilder;
import ddge.utility.meta.type_traits.forward_like;

namespace ddge::renderer {

export class RenderContextBuilder {
public:
    struct CreateInfo {
        const char* application_name{};
        uint32_t    application_version{};
    };

    explicit RenderContextBuilder(
        const CreateInfo&        create_info,
        const vk::raii::Context& context
    );

    template <typename Self_T>
    [[nodiscard]]
    auto instance_builder(this Self_T&& self)
        -> util::meta::forward_like_t<vulkan::InstanceBuilder, Self_T>;

    [[nodiscard]]
    auto build() && -> RenderContext;

private:
    vulkan::InstanceBuilder m_instance_builder;
};

}   // namespace ddge::renderer

namespace ddge::renderer {

template <typename Self_T>
auto RenderContextBuilder::instance_builder(this Self_T&& self)
    -> util::meta::forward_like_t<vulkan::InstanceBuilder, Self_T>
{
    return std::forward_like<Self_T>(self.m_instance_builder);
}

}   // namespace ddge::renderer
