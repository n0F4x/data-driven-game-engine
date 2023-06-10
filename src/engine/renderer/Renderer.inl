namespace engine {

template <renderer::WindowConcept WindowType>
Renderer<WindowType>::Renderer(Window& t_window) : m_swap_chain{ t_window } {}

template <renderer::WindowConcept WindowType>
auto Renderer<WindowType>::create() noexcept -> Renderer::Builder {
    return Renderer::Builder{};
}

////////////////////////////////////////////
///--------------------------------------///
///  Renderer::Builder   IMPLEMENTATION  ///
///--------------------------------------///
////////////////////////////////////////////
template <renderer::WindowConcept WindowType>
auto Renderer<WindowType>::Builder::build(Window& t_window) const
    -> Renderer<WindowType> {
    return Renderer<WindowType>(t_window);
}

}   // namespace engine
