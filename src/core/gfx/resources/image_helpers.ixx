module;

#include <vulkan/vulkan.hpp>

#include "core/renderer/base/resources/Image.hpp"

export module core.gfx.resources.image_helpers;

namespace core::gfx::resources {

export auto transition_image_layout(
    vk::CommandBuffer                   command_buffer,
    renderer::base::Image&              image,
    const renderer::base::Image::State& new_state
) -> renderer::base::Image::State;

}   // namespace core::gfx::resources
