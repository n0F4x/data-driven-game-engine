namespace core::cache {

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource, typename... Args>
auto BasicCache<IdType, ContainerTemplate>::emplace(ID id, Args&&... args)
    -> Handle<Resource>
{
    return lazy_emplace<Resource>(id, [&] {
        return std::forward_as_tuple(std::forward<Args>(args)...);
    });
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource, std::invocable Creator>
auto BasicCache<IdType, ContainerTemplate>::lazy_emplace(ID id, Creator&& create)
    -> Handle<Resource>
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
auto BasicCache<IdType, ContainerTemplate>::find(ID id
) const noexcept -> std::optional<Handle<Resource>>
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
auto BasicCache<IdType, ContainerTemplate>::at(ID id) const -> Handle<Resource>
{
    return m_store.at<ContainerType<Resource>>().at(id).lock();
}

}   // namespace core::cache
