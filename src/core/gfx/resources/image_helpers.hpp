#pragma once

#include "core/renderer/base/resources/Image.hpp"

namespace core::gfx::resources {

auto transition_image_layout(
    vk::CommandBuffer                   command_buffer,
    renderer::base::Image&              image,
    const renderer::base::Image::State& new_state
) -> void;

}   // namespace core::gfx::resources
