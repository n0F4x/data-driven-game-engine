#pragma once

#include <unordered_map>

#include "store/Store.hpp"

#include "Handle.hpp"

namespace core::cache {

template <typename IdType, template <typename...> typename ContainerTemplate>
class BasicCache {
public:
    using ID = IdType;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename Resource, typename... Args>
    auto emplace(ID t_id, Args&&... t_args) -> Handle<Resource>;

    template <typename Resource, std::invocable Creator>
    auto lazy_emplace(ID t_id, Creator&& create) -> Handle<Resource>;

    template <typename Resource>
    [[nodiscard]]
    auto find(ID t_id) const noexcept -> std::optional<Handle<Resource>>;

    template <typename Resource>
    [[nodiscard]]
    auto at(ID t_id) const -> Handle<Resource>;

private:
    ///****************///
    ///  Type aliases  ///
    ///****************///
    template <typename Resource>
    using WeakHandle = std::weak_ptr<Resource>;

    template <typename Resource>
    using ContainerType = ContainerTemplate<IdType, WeakHandle<Resource>>;

    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store;
};

using Cache = BasicCache<size_t, std::unordered_map>;

}   // namespace core::cache

#include "Cache.inl"
