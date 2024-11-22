module;

export module core.renderer.model.Drawable;

import vulkan_hpp;

namespace core::renderer {

export class Drawable {
public:
    virtual ~Drawable() = default;

    virtual auto draw(
        vk::CommandBuffer  graphics_command_buffer,
        vk::PipelineLayout pipeline_layout
    ) const -> void = 0;
};

}   // namespace core::renderer
