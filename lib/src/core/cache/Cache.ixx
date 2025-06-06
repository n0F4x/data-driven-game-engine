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

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource, typename... Args>
auto core::cache::BasicCache<IdType, ContainerTemplate>::emplace(ID id, Args&&... args)
    -> Handle<Resource>
{
    return lazy_emplace<Resource>(id, [&] {
        return std::forward_as_tuple(std::forward<Args>(args)...);
    });
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource, std::invocable Creator>
auto core::cache::BasicCache<IdType, ContainerTemplate>::lazy_emplace(
    ID        id,
    Creator&& create
) -> Handle<Resource>
{
    auto& container{ m_store.emplace<ContainerType<Resource>>() };

    if (const auto iter{ container.find(id) }; iter != container.end()) {
        WeakHandle<Resource>& weak_handle{ iter->second };
        auto                  found_handle{ weak_handle.lock() };
        if (found_handle == nullptr) {
            const auto result{
                make_handle<Resource>(std::invoke(std::forward<Creator>(create)))
            };
            weak_handle = result;
            return result;
        }

        return found_handle;
    }

    const auto result{ make_handle<Resource>(std::invoke(std::forward<Creator>(create))) };
    container.try_emplace(id, result);
    return result;
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto core::cache::BasicCache<IdType, ContainerTemplate>::find(ID id) const noexcept
    -> std::optional<Handle<Resource>>
{
    return m_store.find<ContainerType<Resource>>().and_then(
        [id](const ContainerType<Resource>& container) -> std::optional<Handle<Resource>> {
            const auto iter{ container.find(id) };
            if (iter == container.cend()) {
                return std::nullopt;
            }
            auto result{ iter->second.lock() };
            return result != nullptr ? std::optional{ std::move(result) } : std::nullopt;
        }
    );
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto core::cache::BasicCache<IdType, ContainerTemplate>::at(ID id) const
    -> Handle<Resource>
{
    return m_store.at<ContainerType<Resource>>().at(id).lock();
}
