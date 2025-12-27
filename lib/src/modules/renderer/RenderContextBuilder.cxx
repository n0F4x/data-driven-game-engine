module ddge.modules.renderer.RenderContextBuilder;

import vulkan_hpp;

namespace ddge::renderer {

RenderContextBuilder::RenderContextBuilder(vulkan::InstanceBuilder& instance_builder)
    : m_instance_builder{ instance_builder }
{}

auto RenderContextBuilder::instance_builder() const -> vulkan::InstanceBuilder&
{
    return m_instance_builder;
}

auto RenderContextBuilder::request_default_debug_messenger() -> bool
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

}   // namespace ddge::renderer
