#pragma once

#include <unordered_map>

#include "core/store/Store.hpp"

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
    auto emplace(ID id, Args&&... args) -> Handle<Resource>;

    template <typename Resource, std::invocable Creator>
    auto lazy_emplace(ID id, Creator&& create) -> Handle<Resource>;

    template <typename Resource>
    [[nodiscard]]
    auto find(ID id) const noexcept -> std::optional<Handle<Resource>>;

    template <typename Resource>
    [[nodiscard]]
    auto at(ID id) const -> Handle<Resource>;

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
