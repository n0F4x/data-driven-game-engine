#pragma once

#include <vulkan/vulkan.hpp>

namespace core::renderer {

class Drawable {
public:
    virtual ~Drawable() = default;

    virtual auto draw(
        vk::CommandBuffer  graphics_command_buffer,
        vk::PipelineLayout pipeline_layout
    ) const noexcept -> void = 0;
};

}   // namespace core::renderer
