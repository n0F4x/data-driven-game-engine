#pragma once

#include "RendererImpl.hpp"
#include "SwapChain.hpp"

namespace engine {

template <renderer::WindowConcept WindowType>
class Renderer {
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
    explicit Renderer(const vk::ApplicationInfo& t_app_info, Window& t_window);

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void begin_frame();
    void end_frame();

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::raii::Instance m_instance;
    renderer::RendererImpl m_pimpl;
    SwapChain m_swap_chain;
};

}   // namespace engine

#include "Renderer.inl"
