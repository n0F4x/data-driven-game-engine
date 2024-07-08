#pragma once

#include <expected>
#include <memory>
#include <span>
#include <string>

#include <GLFW/glfw3.h>

#include <gsl-lite/gsl-lite.hpp>

namespace core::window {

class Window {
public:
    [[nodiscard]]
    static auto
        vulkan_instance_extensions() -> const std::vector<gsl_lite::not_null<gsl_lite::czstring>>&;

    explicit Window(uint16_t t_width, uint16_t t_height, const std::string& t_title);

    [[nodiscard]]
    auto get() const noexcept -> GLFWwindow*;

    [[nodiscard]]
    auto create_vulkan_surface(
        VkInstance                   instance,
        const VkAllocationCallbacks* allocation_callbacks
    ) const -> std::expected<VkSurfaceKHR, VkResult>;

private:
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_impl;
};

}   // namespace core::window
