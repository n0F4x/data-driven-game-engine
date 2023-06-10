#pragma once

#include "WindowConcept.hpp"

namespace engine::renderer {

template <WindowConcept WindowType>
class SwapChain {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Window = WindowType;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit SwapChain(Window& t_window);

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Window& m_window;
};

}   // namespace engine::renderer

#include "SwapChain.inl"
