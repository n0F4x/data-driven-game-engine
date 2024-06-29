#pragma once

#include <span>
#include <string>

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

namespace plugins::renderer {

[[nodiscard]]
auto default_required_instance_settings_are_available(const vkb::SystemInfo& t_system_info
) -> bool;

auto enable_default_instance_settings(
    const vkb::SystemInfo& t_system_info,
    vkb::InstanceBuilder&  t_builder
) -> void;

auto log_renderer_setup(const vkb::Device& t_device) -> void;

}   // namespace plugins::renderer
