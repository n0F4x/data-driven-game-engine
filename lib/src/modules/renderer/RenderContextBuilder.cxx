module;

#include <expected>
#include <optional>
#include <utility>

module ddge.modules.renderer.RenderContextBuilder;

import vulkan_hpp;

import ddge.modules.config.engine_name;
import ddge.modules.config.engine_version;
import ddge.modules.vulkan.default_debug_messenger_callback;
import ddge.modules.vulkan.InstanceInjection;
import ddge.modules.vulkan.result.check_result;
import ddge.utility.containers.Lazy;

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
