#include <stdexcept>
#include <string>

namespace engine::asset {

template <typename Asset>
auto AssetRegistry::emplace(Asset&& t_asset, entt::id_type t_id) noexcept -> Asset&
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

template <typename Asset>
auto AssetRegistry::find(entt::id_type t_id) const noexcept -> tl::optional<const Asset&>
{
    return m_store.find<ContainerType<Asset>>().and_then(
        [t_id](const ContainerType<Asset>& t_container) -> tl::optional<const Asset&> {
            const auto iter{ t_container.find(t_id) };
            if (iter == t_container.cend()) {
                return tl::nullopt;
            }
            return iter->second;
        }
    );
}

template <typename Asset>
auto AssetRegistry::at(entt::id_type t_id) -> Asset&
{
    using namespace std::string_literals;

    auto& container{ m_store.at<ContainerType<Asset>>() };

    const auto iter{ container.find(t_id) };
    if (iter == container.cend()) {
        throw std::out_of_range{ "AssetRegistry::at is out of range for type `"s
                                     .append(entt::type_id<Asset>().name())
                                     .append("`") };
    }

    return iter->second;
}

template <typename Asset>
auto AssetRegistry::at(entt::id_type t_id) const -> const Asset&
{
    using namespace std::string_literals;

    const auto& container{ m_store.at<ContainerType<Asset>>() };

    const auto iter{ container.find(t_id) };
    if (iter == container.cend()) {
        throw std::out_of_range{ "AssetRegistry::at is out of range for type `"s
                                     .append(entt::type_id<Asset>().name())
                                     .append("`") };
    }

    return iter->second;
}

}   // namespace engine::asset
