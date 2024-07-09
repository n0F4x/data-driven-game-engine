#pragma once

#include <functional>

#include <vulkan/vulkan_core.h>

#include "core/renderer/base/swapchain/SwapchainHolder.hpp"
#include "plugins/renderer/Options.hpp"
#include "plugins/renderer/SurfaceProvider.hpp"

#include "app.hpp"

namespace plugins {

class Renderer {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(Store& store) const -> void;

    template <renderer::SurfaceProviderConcept SurfaceProvider>
    auto operator()(Store& store, const renderer::Options<SurfaceProvider>& options) const
        -> void;
};

}   // namespace plugins

#include "Renderer.inl"
