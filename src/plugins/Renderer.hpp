#pragma once

#include <functional>

#include <vulkan/vulkan_core.h>

#include "app/Plugin.hpp"
#include "core/renderer/base/swapchain/SwapchainHolder.hpp"
#include "plugins/renderer/Options.hpp"
#include "plugins/renderer/SurfaceProvider.hpp"

namespace plugins {

class Renderer {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Builder& builder) const -> void;

    template <renderer::SurfaceProviderConcept SurfaceProvider>
    auto operator()(
        App::Builder&                             builder,
        const renderer::Options<SurfaceProvider>& options
    ) const -> void;
};

static_assert(PluginConcept<Renderer>);

}   // namespace plugins

#include "Renderer.inl"
