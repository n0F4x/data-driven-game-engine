#pragma once

#include <concepts>
#include <optional>

#include <vulkan/vulkan_core.h>

#include "store/Store.hpp"

#include "DependencyProvider.hpp"

namespace plugins::renderer {

template <typename SurfaceProvider>
concept SurfaceProviderConcept = requires(
                                     SurfaceProvider              surface_provider,
                                     const Store&                 store,
                                     const VkInstance             instance,
                                     const VkAllocationCallbacks* allocation_callbacks
                                 ) {
    {
        surface_provider(store, instance, allocation_callbacks)
    } -> std::same_as<std::optional<VkSurfaceKHR>>;
} && std::derived_from<SurfaceProvider, DependencyProvider>;

}   // namespace plugins::renderer
