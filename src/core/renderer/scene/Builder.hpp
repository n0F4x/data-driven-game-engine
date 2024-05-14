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

    auto set_cache(cache::Cache& cache) noexcept -> Builder&;

    auto add_model(const cache::Handle<graphics::Model>& model, const Effect& effect)
        -> Builder&;
    auto add_model(cache::Handle<graphics::Model>&& model, const Effect& effect)
        -> Builder&;

    [[nodiscard]]
    auto build(vk::Device device, const Allocator& allocator, vk::RenderPass render_pass)
        const -> std::packaged_task<Scene(vk::CommandBuffer)>;

private:
    std::optional<std::reference_wrapper<cache::Cache>> m_cache;
    std::vector<ModelInfo>                              m_models;
};

}   // namespace core::renderer
