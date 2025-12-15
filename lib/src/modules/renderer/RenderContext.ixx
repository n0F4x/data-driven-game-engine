export module ddge.modules.renderer.RenderContext;

import vulkan_hpp;

namespace ddge::renderer {

export struct RenderContext {
    vk::raii::Instance               instance;
    vk::raii::DebugUtilsMessengerEXT default_debug_messenger;
    vk::raii::PhysicalDevice         physical_device;
    vk::raii::Device                 device;
};

}   // namespace ddge::renderer
