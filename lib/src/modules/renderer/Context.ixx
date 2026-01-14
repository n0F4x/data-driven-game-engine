module;

#include <functional>

export module ddge.modules.renderer.Context;

import vulkan_hpp;

import ddge.modules.vulkan.Device;

namespace ddge::renderer {

export struct Context {
    std::reference_wrapper<const vk::raii::Instance> instance;
    vk::raii::DebugUtilsMessengerEXT                 default_debug_messenger;
    vulkan::Device                                   device;
};

}   // namespace ddge::renderer
