export module ddge.modules.renderer.RenderContext;

import vulkan_hpp;

import ddge.modules.vulkan.Device;

namespace ddge::renderer {

export struct RenderContext {
    vk::raii::Instance               instance;
    vk::raii::DebugUtilsMessengerEXT default_debug_messenger;
    vulkan::Device                   device;
};

}   // namespace ddge::renderer
