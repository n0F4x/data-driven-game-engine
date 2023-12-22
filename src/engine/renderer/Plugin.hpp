#pragma once

#include <functional>

#include <vulkan/vulkan_core.h>

#include "engine/common/Plugin.hpp"

#include "Swapchain.hpp"

namespace engine::renderer {

using SurfaceCreator = std::function<
    VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>;

using FramebufferSizeGetterCreator =
    std::function<Swapchain::FramebufferSizeGetter(Store&)>;

class Plugin {
public:
    ///--------------------///
    ///  Static variables  ///
    ///--------------------///
    static SurfaceCreator create_surface;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(
        Store&                              t_store,
        const SurfaceCreator&               t_create_surface = create_surface,
        const FramebufferSizeGetterCreator& t_create_framebuffer_size_getter =
            nullptr
    ) const noexcept -> void;
};

static_assert(PluginConcept<Plugin> && PluginConcept<Plugin, const SurfaceCreator&>);

}   // namespace engine::renderer
