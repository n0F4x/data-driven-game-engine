#pragma once

#include <memory>
#include <unordered_map>

#include <gsl/pointers>

#include <entt/core/fwd.hpp>

#include "engine/app/Store.hpp"

namespace engine {

template <typename IdType, template <typename...> typename ContainerTemplate>
class BasicCache {
public:
    using Id = IdType;

    template <typename Resource>
    using Handle = gsl::not_null<std::shared_ptr<Resource>>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename Resource>
    auto emplace(Id t_id, auto&&... t_args) noexcept -> Handle<Resource>;

    template <typename Resource>
    [[nodiscard]] auto find(Id t_id) noexcept -> tl::optional<Handle<Resource>>;

    template <typename Resource>
    [[nodiscard]] auto at(Id t_id) -> Handle<Resource>;

    template <typename Resource>
    auto remove(Id t_id) noexcept -> tl::optional<Handle<Resource>>;

private:
    ///****************///
    ///  Type aliases  ///
    ///****************///
    template <typename Resource>
    using ContainerType = ContainerTemplate<IdType, Handle<Resource>>;

    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store;
};

using Cache = BasicCache<entt::id_type, std::unordered_map>;

}   // namespace engine

#include "Cache.inl"
