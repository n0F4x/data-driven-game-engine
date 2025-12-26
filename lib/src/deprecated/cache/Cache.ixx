module;

#include <memory>
#include <unordered_map>

export module ddge.deprecated.cache.Cache;

import ddge.utility.containers.store.Store;

import ddge.deprecated.cache.Handle;

namespace ddge::cache {

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
    ddge::util::store::Store m_store;
};

export using Cache = BasicCache<std::size_t, std::unordered_map>;

}   // namespace ddge::cache

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource, typename... Args>
auto ddge::cache::BasicCache<IdType, ContainerTemplate>::emplace(ID id, Args&&... args)
    -> Handle<Resource>
{
    return lazy_emplace<Resource>(id, [&] {
        return std::forward_as_tuple(std::forward<Args>(args)...);
    });
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource, std::invocable Creator>
auto ddge::cache::BasicCache<IdType, ContainerTemplate>::lazy_emplace(
    ID        id,
    Creator&& create
) -> Handle<Resource>
{
    auto& container{ m_store.emplace<ContainerType<Resource>>() };

    if (const auto iter{ container.find(id) }; iter != container.end()) {
        WeakHandle<Resource>& weak_handle{ iter->second };
        Handle<Resource>      found_handle{ weak_handle.lock() };
        if (found_handle == nullptr) {
            const auto result{
                make_handle<Resource>(std::invoke(std::forward<Creator>(create)))
            };
            weak_handle = result;
            return result;
        }

        return std::move(found_handle);
    }

    Handle<Resource> result{
        make_handle<Resource>(std::invoke(std::forward<Creator>(create)))
    };
    container.try_emplace(id, result);
    return result;
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto ddge::cache::BasicCache<IdType, ContainerTemplate>::find(ID id) const noexcept
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
auto ddge::cache::BasicCache<IdType, ContainerTemplate>::at(ID id) const
    -> Handle<Resource>
{
    return m_store.at<ContainerType<Resource>>().at(id).lock();
}
