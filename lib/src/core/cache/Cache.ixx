module;

#include <memory>
#include <unordered_map>

export module core.cache.Cache;

import core.store.Store;

import core.cache.Handle;

namespace core::cache {

export template <typename IdType, template <typename...> typename ContainerTemplate>
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
    core::store::Store m_store;
};

export using Cache = BasicCache<size_t, std::unordered_map>;

}   // namespace core::cache

#include "Cache.inl"
