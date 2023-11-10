#pragma once

#include <functional>

#include <vulkan/vulkan_core.h>

#include "engine/common/Plugin.hpp"

namespace engine::renderer {

using SurfaceCreator = std::function<
    VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>;

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
        const SurfaceCreator& t_create_surface = default_surface_creator
    ) const noexcept -> void;
};

static_assert(PluginConcept<Plugin> && PluginConcept<Plugin, const SurfaceCreator&>);

}   // namespace engine::renderer
