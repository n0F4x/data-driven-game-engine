export module ddge.modules.renderer.RenderContext;

import vulkan_hpp;

namespace ddge::renderer {

export struct RenderContext {
    vk::raii::Instance               instance;
    vk::raii::DebugUtilsMessengerEXT default_debug_messenger;
};

}   // namespace ddge::renderer
