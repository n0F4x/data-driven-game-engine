#pragma once

#include <functional>

#include <vulkan/vulkan_core.h>

#include "app/Plugin.hpp"
#include "core/renderer/base/swapchain/SwapchainHolder.hpp"

namespace plugins {

class Renderer {
public:
    using SurfaceCreator =
        std::function<VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>;

    using FramebufferSizeGetterCreator =
        std::function<core::renderer::SwapchainHolder::FramebufferSizeGetter(Store&)>;

    class Options;

    ///--------------------///
    ///  Static variables  ///
    ///--------------------///
    [[nodiscard]]
    static auto create_default_surface(Store&, vk::Instance, const VkAllocationCallbacks*)
        -> vk::SurfaceKHR;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(app::App::Builder& builder) const -> void;
    auto operator()(app::App::Builder& builder, const Options& options) const -> void;
};

static_assert(app::PluginConcept<Renderer>);

}   // namespace plugins

#include "plugins/renderer/Options.hpp"
