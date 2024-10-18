#pragma once

#include <ranges>
#include <vector>

#include "core/cache/Handle.hpp"
#include "core/gltf/Model.hpp"

#include "Scene.hpp"

namespace core::renderer {

class Scene::Builder {
public:
    auto set_cache(cache::Cache& cache) noexcept -> Builder&;

    auto add_model(const cache::Handle<const gltf::Model>& model) -> Builder&;
    auto add_model(cache::Handle<const gltf::Model>&& model) -> Builder&;

    [[nodiscard]]
    auto build(
        const base::Device&    device,
        const base::Allocator& allocator,
        vk::RenderPass         render_pass
    ) const -> std::packaged_task<Scene(vk::CommandBuffer)>;

private:
    std::optional<std::reference_wrapper<cache::Cache>> m_cache;
    std::vector<cache::Handle<const gltf::Model>>       m_models;
};

}   // namespace core::renderer
