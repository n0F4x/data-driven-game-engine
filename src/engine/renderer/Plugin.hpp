#pragma once

#include <functional>

#include <vulkan/vulkan_core.h>

#include "engine/app/Plugin.hpp"
#include "engine/renderer/base/Swapchain.hpp"

namespace engine::renderer {

using SurfaceCreator =
    std::function<VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>;

using FramebufferSizeGetterCreator =
    std::function<Swapchain::FramebufferSizeGetter(Store&)>;

class Plugin {
public:
    ///--------------------///
    ///  Static variables  ///
    ///--------------------///
    static SurfaceCreator create_default_surface;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(
        App::Builder&                       t_builder,
        const SurfaceCreator&               t_create_surface = create_default_surface,
        const FramebufferSizeGetterCreator& t_create_framebuffer_size_getter = nullptr
    ) const noexcept -> void;
};

static_assert(PluginConcept<Plugin>);
static_assert(PluginConcept<Plugin, const SurfaceCreator&>);
static_assert(PluginConcept<
              Plugin,
              const SurfaceCreator&,
              const FramebufferSizeGetterCreator&>);

}   // namespace engine::renderer
