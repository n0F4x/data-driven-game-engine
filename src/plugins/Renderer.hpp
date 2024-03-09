#pragma once

#include <functional>

#include <vulkan/vulkan_core.h>

#include "app/core/Plugin.hpp"
#include "engine/renderer/base/Swapchain.hpp"

namespace plugins {

class Renderer {
public:
    using SurfaceCreator =
        std::function<VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>;

    using FramebufferSizeGetterCreator =
        std::function<engine::renderer::Swapchain::FramebufferSizeGetter(Store&)>;

    ///--------------------///
    ///  Static variables  ///
    ///--------------------///
    static SurfaceCreator create_default_surface;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(
        app::App::Builder&                       t_builder,
        const SurfaceCreator&               t_create_surface = create_default_surface,
        const FramebufferSizeGetterCreator& t_create_framebuffer_size_getter = nullptr
    ) const noexcept -> void;
};

static_assert(app::PluginConcept<Renderer>);
static_assert(app::PluginConcept<Renderer, const Renderer::SurfaceCreator&>);
static_assert(app::PluginConcept<
              Renderer,
              const Renderer::SurfaceCreator&,
              const Renderer::FramebufferSizeGetterCreator&>);

}   // namespace app::plugins
