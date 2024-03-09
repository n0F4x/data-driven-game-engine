#pragma once

#include <memory>
#include <unordered_map>

#include <gsl/pointers>

#include <entt/core/fwd.hpp>

#include "engine/common/Store.hpp"

namespace engine::asset {

template <typename IdType, template <typename...> typename ContainerTemplate>
class BasicRegistry {
public:
    using Id = IdType;

    template <typename Resource>
    using Handle = gsl::not_null<std::shared_ptr<Resource>>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename Resource>
    auto emplace(Id t_id, auto&&... t_args) noexcept
        -> Handle<Resource>;

    template <typename Resource>
    [[nodiscard]] auto find(Id t_id) noexcept -> tl::optional<Handle<Resource>>;

    template <typename Resource>
    [[nodiscard]] auto at(Id t_id) -> Handle<Resource>;

    template <typename Resource>
    auto remove(Id t_id) -> tl::optional<Handle<Resource>>;

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

using Registry = BasicRegistry<entt::id_type, std::unordered_map>;

}   // namespace engine::asset

#include "Registry.inl"
