export module ddge.modules.renderer.RenderContext;

import vulkan_hpp;

import ddge.modules.vulkan.QueueGroup;

namespace ddge::renderer {

export struct RenderContext {
    vk::raii::Instance               instance;
    vk::raii::DebugUtilsMessengerEXT default_debug_messenger;
    vk::raii::PhysicalDevice         physical_device;
    vk::raii::Device                 device;
    vulkan::QueueGroup               queue_group;
};

}   // namespace ddge::renderer
