#pragma once

#include "engine/core/renderer.hpp"

namespace engine {

class Renderer final : public RendererInterface<Renderer> {
public:
    Renderer() : RendererInterface<Renderer>{} {}
    Renderer(Renderer&&) noexcept = default;
    Renderer& operator=(Renderer&&) noexcept = default;

    void render() {}
};

}   // namespace engine
