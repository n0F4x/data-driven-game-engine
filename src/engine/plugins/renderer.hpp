#pragma once

#include "engine/common/app/Plugin.hpp"
#include "engine/plugins/renderer/Renderer.hpp"

namespace engine::renderer {

class RendererPlugin {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto set_context(App::Context& t_context) const noexcept -> void;
};

static_assert(PluginConcept<RendererPlugin>);

}   // namespace engine::renderer
