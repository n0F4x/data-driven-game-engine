#pragma once

#include <functional>

#include <vulkan/vulkan_core.h>

#include "engine/common/Plugin.hpp"

#include "Swapchain.hpp"

namespace engine::renderer {

using SurfaceCreator = std::function<
    VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>;

using FramebufferSizeGetter =
    std::function<Swapchain::FramebufferSizeGetter(Store&)>;

class Plugin {
public:
    ///--------------------///
    ///  Static variables  ///
    ///--------------------///
    static SurfaceCreator default_surface_creator;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(
        Store&                t_store,
        const SurfaceCreator& t_create_surface       = default_surface_creator,
        FramebufferSizeGetter t_get_framebuffer_size = nullptr
    ) const noexcept -> void;
};

static_assert(PluginConcept<Plugin> && PluginConcept<Plugin, const SurfaceCreator&>);

}   // namespace engine::renderer
