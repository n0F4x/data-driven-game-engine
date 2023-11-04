namespace engine::asset_manager {

template <typename Asset>
auto AssetRegistry::emplace(Asset&& t_asset, entt::id_type t_id) noexcept
    -> Asset&
{
    return m_store.emplace<ContainerType<Asset>>()
        .try_emplace(t_id, std::forward<Asset>(t_asset))
        .first->second;
}

template <typename Asset>
auto AssetRegistry::find(entt::id_type t_id) noexcept -> tl::optional<Asset&>
{
    return m_store.find<ContainerType<Asset>>().and_then(
        [t_id](ContainerType<Asset>& t_container) -> tl::optional<Asset&> {
            const auto iter{ t_container.find(t_id) };
            if (iter == t_container.cend()) {
                return tl::nullopt;
            }
            return iter->second;
        }
    );
}

}   // namespace engine::asset_manager
