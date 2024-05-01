#pragma once

#include <ranges>
#include <vector>

#include "core/cache/Handle.hpp"
#include "core/graphics/model/Model.hpp"

#include "Scene.hpp"

namespace core::renderer {

class Scene::Builder {
public:
    struct ModelInfo {
        cache::Handle<graphics::Model> handle;
        Effect                         effect;
    };

    auto add_model(const cache::Handle<graphics::Model>& t_model, const Effect& t_effect)
        -> Builder&;
    auto add_model(cache::Handle<graphics::Model>&& t_model, const Effect& t_effect) -> Builder&;

    [[nodiscard]]
    auto build(
        vk::Device        t_device,
        const Allocator&  t_allocator,
        vk::CommandBuffer t_transfer_command_buffer,
        vk::RenderPass    t_render_pass
    ) const -> Scene;

private:
    std::vector<ModelInfo> m_models;
};

}   // namespace core::renderer
