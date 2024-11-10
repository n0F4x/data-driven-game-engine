#pragma once

#include <vulkan/vulkan.hpp>

class StoreView;

namespace core::window {

class Window;

}   // namespace core::window

namespace core::renderer::base {

class Instance;

}   // namespace core::renderer::base

namespace plugins::renderer {

class SurfacePlugin {
public:
    [[nodiscard]]
    auto operator()(
        const core::window::Window&           window,
        const core::renderer::base::Instance& instance
    ) const -> vk::UniqueSurfaceKHR;

    static auto setup(StoreView plugins) -> void;
};

}   // namespace plugins::renderer
