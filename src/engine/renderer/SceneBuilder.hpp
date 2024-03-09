#pragma once

#include <functional>
#include <set>
#include <vector>

#include <tl/optional.hpp>

#include <entt/core/fwd.hpp>

#include "engine/common/Cache.hpp"
#include "engine/renderer/base/Allocator.hpp"
#include "engine/renderer/model/ImageLoader.hpp"
#include "engine/renderer/model/ModelLoader.hpp"

#include "Scene.hpp"

namespace engine::renderer {

class SceneBuilder {
public:
    explicit SceneBuilder(Cache& t_cache) noexcept;

    auto load_model(auto&&... t_args) -> tl::optional<entt::id_type>;

    [[nodiscard]] auto build(const renderer::Allocator& t_allocator) && -> Scene;

private:
    std::reference_wrapper<Cache> m_cache;
    std::set<entt::id_type>       m_cached_models;
    //    std::vector<>                  m_models;
};

}   // namespace engine::renderer

#include "SceneBuilder.inl"
