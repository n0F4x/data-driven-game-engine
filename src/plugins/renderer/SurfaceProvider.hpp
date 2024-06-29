#pragma once

#include <vulkan/vulkan_core.h>

#include "DependencyProvider.hpp"

namespace plugins::renderer {

template <typename SurfaceProvider>
concept SurfaceProviderConcept = requires(
                                     SurfaceProvider              surface_provider,
                                     const VkInstance             instance,
                                     const VkAllocationCallbacks* allocation_callbacks
                                 ) {
    {
        surface_provider(instance, allocation_callbacks)
    } -> std::same_as<std::expected<VkSurfaceKHR, VkResult>>;
} && std::derived_from<SurfaceProvider, DependencyProvider>;

}   // namespace plugins::renderer
