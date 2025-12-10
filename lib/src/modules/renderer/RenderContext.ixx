export module ddge.modules.renderer.RenderContext;

import vulkan_hpp;

namespace ddge::renderer {

export struct RenderContext {
    vk::raii::Instance instance;
};

}   // namespace ddge::renderer
