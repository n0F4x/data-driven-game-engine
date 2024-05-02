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
    auto add_model(cache::Handle<graphics::Model>&& t_model, const Effect& t_effect)
        -> Builder&;

    [[nodiscard]]
    auto build(
        vk::Device        t_device,
        const Allocator&  t_allocator,
        vk::RenderPass    t_render_pass
    ) const -> std::packaged_task<Scene(vk::CommandBuffer)>;

private:
    std::vector<ModelInfo> m_models;
};

}   // namespace core::renderer
