#pragma once

#include <memory>
#include <string>

#include <GLFW/glfw3.h>

namespace core::window {

class Window {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Window(uint16_t t_width, uint16_t t_height, const std::string& title);

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto get() const noexcept -> GLFWwindow*;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_impl;
};

}   // namespace core::window
