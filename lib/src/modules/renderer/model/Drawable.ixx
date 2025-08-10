module;

export module ddge.modules.renderer.model.Drawable;

import vulkan_hpp;

namespace ddge::renderer {

export class Drawable {
public:
    virtual ~Drawable() = default;

    virtual auto draw(
        vk::CommandBuffer  graphics_command_buffer,
        vk::PipelineLayout pipeline_layout
    ) const -> void = 0;
};

}   // namespace ddge::renderer
