namespace core::cache {

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource, typename... Args>
auto BasicCache<IdType, ContainerTemplate>::emplace(ID t_id, Args&&... t_args)
    -> Handle<Resource>
{
    auto& container{ m_store.emplace<ContainerType<Resource>>() };

    if (const auto iter{ container.find(t_id) }; iter != container.end()) {
        WeakHandle<Resource>& weak_handle{ iter->second };
        auto                  found_handle{ weak_handle.lock() };
        if (found_handle == nullptr) {
            const auto result{ make_handle<Resource>(std::forward<Args>(t_args)...) };
            weak_handle = result;
            return result;
        }

        return found_handle;
    }

    const auto result{ make_handle<Resource>(std::forward<Args>(t_args)...) };
    container.try_emplace(t_id, result);
    return result;
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicCache<IdType, ContainerTemplate>::find(ID t_id
) const noexcept -> std::optional<Handle<Resource>>
{
    return m_store.find<ContainerType<Resource>>().and_then(
        [t_id](const ContainerType<Resource>& t_container
        ) -> std::optional<Handle<Resource>> {
            const auto iter{ t_container.find(t_id) };
            if (iter == t_container.cend()) {
                return std::nullopt;
            }
            auto result{ iter->second.lock() };
            return result != nullptr ? std::optional{ std::move(result) } : std::nullopt;
        }
    );
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicCache<IdType, ContainerTemplate>::at(ID t_id) const -> Handle<Resource>
{
    return m_store.at<ContainerType<Resource>>().at(t_id).lock();
}

}   // namespace core::cache
