#include "SwapChain.hpp"

namespace engine::renderer {

template <WindowConcept WindowType>
SwapChain<WindowType>::SwapChain(Window& t_window) : m_window{ t_window }
{
}

}   // namespace engine::renderer
