#pragma once

#include <future>
#include <ranges>
#include <vector>

#include "Scene.hpp"

import core.cache.Cache;
import core.cache.Handle;

import core.gltf.Model;

namespace core::renderer {

namespace base {

class Allocator;
class Device;

}   // namespace base

class Scene::Builder {
public:
    auto set_cache(cache::Cache& cache) noexcept -> Builder&;

    auto add_model(const cache::Handle<const gltf::Model>& model) -> Builder&;
    auto add_model(cache::Handle<const gltf::Model>&& model) -> Builder&;

    [[nodiscard]]
    auto build(
        const base::Device&    device,
        const base::Allocator& allocator,
        vk::RenderPass         render_pass,
        bool                   use_virtual_images
    ) const -> std::packaged_task<Scene(vk::CommandBuffer)>;

private:
    std::optional<std::reference_wrapper<cache::Cache>> m_cache;
    std::vector<cache::Handle<const gltf::Model>>       m_models;
};

}   // namespace core::renderer
