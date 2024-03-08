#include <stdexcept>
#include <string>

namespace engine::asset {

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicRegistry<IdType, ContainerTemplate>::emplace(
    entt::id_type t_id,
    Resource&&    t_resource
) noexcept -> Resource&
{
    return m_store.emplace<ContainerType<Resource>>()
        .try_emplace(t_id, std::forward<Resource>(t_resource))
        .first->second;
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicRegistry<IdType, ContainerTemplate>::find(entt::id_type t_id) noexcept
    -> tl::optional<Resource&>
{
    return m_store.find<ContainerType<Resource>>().and_then(
        [t_id](ContainerType<Resource>& t_container) -> tl::optional<Resource&> {
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
auto BasicRegistry<IdType, ContainerTemplate>::find(entt::id_type t_id) const noexcept
    -> tl::optional<const Resource&>
{
    return m_store.find<ContainerType<Resource>>().and_then(
        [t_id](const ContainerType<Resource>& t_container
        ) -> tl::optional<const Resource&> {
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
auto BasicRegistry<IdType, ContainerTemplate>::at(entt::id_type t_id) -> Resource&
{
    return m_store.at<ContainerType<Resource>>().at(t_id);
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicRegistry<IdType, ContainerTemplate>::at(entt::id_type t_id) const
    -> const Resource&
{
    return m_store.at<ContainerType<Resource>>().at(t_id);
}

}   // namespace engine::asset
