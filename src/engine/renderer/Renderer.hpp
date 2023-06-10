#pragma once

#include "SwapChain.hpp"

namespace engine {

template <renderer::WindowConcept WindowType>
class Renderer final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Window = WindowType;
    using SwapChain = renderer::SwapChain<Window>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Renderer(Window& t_window);

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void begin_frame();
    void end_frame();

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    SwapChain m_swap_chain;
};

template <renderer::WindowConcept WindowType>
class Renderer<WindowType>::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build(Window& t_window) const -> Renderer<WindowType>;
};

}   // namespace engine

#include "Renderer.inl"
