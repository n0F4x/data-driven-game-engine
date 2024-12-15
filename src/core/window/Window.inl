namespace core::window {

template <std::invocable<utils::Size2i> Callback>
auto Window::set_framebuffer_size_callback(Callback&& callback) -> void
{
    m_framebuffer_resized = std::forward<Callback>(callback);

    glfwSetFramebufferSizeCallback(
        m_impl.get(),
        [](GLFWwindow* window, const int width, const int height) {
            static_cast<Window*>(glfwGetWindowUserPointer(window))
                ->m_framebuffer_resized(utils::Size2i{ width, height });
        }
    );
}

template <typename Self>
auto Window::set_title(this Self&& self, const gsl_lite::czstring title) -> Self
{
    glfwSetWindowTitle(self.m_impl.get(), title);
    return std::forward<Self>(self);
}

}   // namespace core::window
