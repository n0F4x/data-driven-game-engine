#pragma once

#include <concepts>
#include <optional>

#include <vulkan/vulkan_core.h>

#include "app/App.hpp"

#include "DependencyProvider.hpp"

namespace plugins::renderer {

template <typename SurfaceProvider>
concept SurfaceProviderConcept = requires(
                                     SurfaceProvider              surface_provider,
                                     const App&                   app,
                                     const VkInstance             instance,
                                     const VkAllocationCallbacks* allocation_callbacks
                                 ) {
    {
        surface_provider(app, instance, allocation_callbacks)
    } -> std::same_as<std::optional<VkSurfaceKHR>>;
} && std::derived_from<SurfaceProvider, DependencyProvider>;

}   // namespace plugins::renderer
