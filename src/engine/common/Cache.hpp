#pragma once

#include <unordered_map>

#include "store/Store.hpp"

#include "Handle.hpp"
#include "ID.hpp"

namespace engine {

template <typename IdType, template <typename...> typename ContainerTemplate>
class BasicCache {
public:
    using ID = IdType;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename Resource>
    auto emplace(ID t_id, auto&&... t_args) noexcept -> Handle<Resource>;

    template <typename Resource>
    [[nodiscard]] auto find(ID t_id) noexcept -> tl::optional<Handle<Resource>>;

    template <typename Resource>
    [[nodiscard]] auto at(ID t_id) -> Handle<Resource>;

    template <typename Resource>
    auto remove(ID t_id) noexcept -> tl::optional<Handle<Resource>>;

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

using Cache = BasicCache<ID, std::unordered_map>;

}   // namespace engine

#include "Cache.inl"
