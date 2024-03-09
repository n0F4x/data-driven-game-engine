namespace engine {

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicCache<IdType, ContainerTemplate>::emplace(ID t_id, auto&&... t_args) noexcept
    -> Handle<Resource>
{
    return m_store.emplace<ContainerType<Resource>>()
        .try_emplace(
            t_id, std::make_shared<Resource>(std::forward<decltype(t_args)>(t_args)...)
        )
        .first->second;
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicCache<IdType, ContainerTemplate>::find(ID t_id) noexcept
    -> tl::optional<Handle<Resource>>
{
    return m_store.find<ContainerType<Resource>>().and_then(
        [t_id](ContainerType<Resource>& t_container) -> tl::optional<Handle<Resource>> {
            const auto iter{ t_container.find(t_id) };
            if (iter == t_container.cend()) {
                return tl::nullopt;
            }
            return iter->second;
        }
    );
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicCache<IdType, ContainerTemplate>::at(ID t_id) -> Handle<Resource>
{
    return m_store.at<ContainerType<Resource>>().at(t_id);
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicCache<IdType, ContainerTemplate>::remove(ID t_id) noexcept
    -> tl::optional<Handle<Resource>>
{
    return m_store.find<ContainerType<Resource>>().and_then(
        [t_id](ContainerType<Resource>& t_container) -> tl::optional<Handle<Resource>> {
            const auto iter{ t_container.find(t_id) };
            if (iter == t_container.cend()) {
                return tl::nullopt;
            }

            t_container.erase(iter);

            return iter->second;
        }
    );
}

}   // namespace engine
