namespace core::window {

template <std::invocable<Size2i> Callback>
auto Window::set_framebuffer_size_callback(Callback&& callback) -> void
{
    m_framebuffer_resized = std::forward<Callback>(callback);

    glfwSetFramebufferSizeCallback(
        m_impl.get(),
        [](GLFWwindow* window, const int width, const int height) {
            static_cast<Window*>(glfwGetWindowUserPointer(window))
                ->m_framebuffer_resized(Size2i{ width, height });
        }
    );
}

}   // namespace core::window
