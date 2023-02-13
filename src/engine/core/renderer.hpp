#pragma once

#include <concepts>

namespace engine {

template <class RendererType>
concept RendererConcept = requires(RendererType renderer) {
                              requires std::constructible_from<RendererType>;
                              requires std::movable<RendererType>;
                              renderer.render();
                          };

template <class DerivedType>
class RendererInterface {
public:
    RendererInterface()
        requires(RendererConcept<DerivedType>)
    = default;
};

}   // namespace engine
