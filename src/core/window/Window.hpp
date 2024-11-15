#pragma once

#include <expected>
#include <functional>
#include <memory>

#include <gsl-lite/gsl-lite.hpp>

#include <glm/ext/vector_double2.hpp>

#include <GLFW/glfw3.h>

#include "core/utility/Size.hpp"

#include "CursorMode.hpp"
#include "Key.hpp"

namespace core::window {

class Window {
public:
    [[nodiscard]]
    static auto vulkan_instance_extensions()
        -> const std::vector<gsl_lite::not_null<gsl_lite::czstring>>&;

    explicit Window(const Size2i& size, gsl_lite::czstring title);
    Window(const Window&) = delete;
    Window(Window&&) noexcept;

    auto operator=(const Window&) -> Window& = delete;
    auto operator=(Window&&) noexcept -> Window&;

    /***********/
    /* Getters */
    /***********/
    [[nodiscard]]
    auto get() const noexcept -> GLFWwindow*;
    [[nodiscard]]
    auto should_close() const -> bool;
    [[nodiscard]]
    auto size() const -> Size2i;
    [[nodiscard]]
    auto framebuffer_size() const -> Size2i;
    [[nodiscard]]
    auto title() const -> std::string_view;

    /***********/
    /* Setters */
    /***********/
    template <std::invocable<Size2i> Callback>
    auto set_framebuffer_size_callback(Callback&& callback) -> void;
    template<typename Self>
    auto set_title(this Self&&, gsl_lite::czstring title) -> Self;

    /************/
    /* KeyBoard */
    /************/
    [[nodiscard]]
    auto key_pressed(Key key) const -> bool;

    /**********/
    /* Cursor */
    /**********/
    [[nodiscard]]
    auto cursor_position() const -> glm::dvec2;
    auto set_cursor_position(const glm::dvec2& position) const -> void;
    auto set_cursor_mode(CursorMode cursor_mode) const -> void;

    /**********/
    /* Vulkan */
    /**********/
    [[nodiscard]]
    auto create_vulkan_surface(
        VkInstance                   instance,
        const VkAllocationCallbacks* allocation_callbacks
    ) const -> std::expected<VkSurfaceKHR, VkResult>;

private:
    gsl_lite::not_null<std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>> m_impl;
    std::function<void(Size2i)> m_framebuffer_resized;
};

}   // namespace core::window

#include "Window.inl"
