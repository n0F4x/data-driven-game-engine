module;

#include <utility>

module ddge.modules.renderer.RenderContextBuilder;

import vulkan_hpp;

import ddge.modules.config.engine_name;
import ddge.modules.config.engine_version;
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

RenderContextBuilder::RenderContextBuilder(
    const CreateInfo&        create_info,
    const vk::raii::Context& context
)
    : m_instance_builder{ make_instance_create_info(create_info), context }
{}

auto ddge::renderer::RenderContextBuilder::build() && -> RenderContext
{
    vk::raii::Instance instance{ std::move(m_instance_builder).build() };

    return RenderContext{
        .instance = std::move(instance),
    };
}

}   // namespace ddge::renderer
