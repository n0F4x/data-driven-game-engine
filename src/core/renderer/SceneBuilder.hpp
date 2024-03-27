#pragma once

#include <vector>

#include <tl/optional.hpp>

#include "core/common/Cache.hpp"
#include "core/renderer/base/Allocator.hpp"
#include "core/renderer/model/Model.hpp"

#include "Scene.hpp"

namespace core::renderer {

class SceneBuilder {
public:
    explicit SceneBuilder(Cache& t_cache) noexcept;

    auto load_model(auto&&... t_args) -> tl::optional<Handle<Model>>;

    [[nodiscard]] auto build(const Allocator& t_allocator) && -> Scene;

private:
    std::reference_wrapper<Cache> m_cache;
    std::vector<Handle<Model>>    m_models;
};

}   // namespace core::renderer

#include "SceneBuilder.inl"
