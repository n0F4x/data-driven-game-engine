#include <stdexcept>
#include <string>

namespace engine::asset {

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicRegistry<IdType, ContainerTemplate>::emplace(Id t_id, auto&&... t_args) noexcept
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
auto BasicRegistry<IdType, ContainerTemplate>::find(Id t_id) noexcept
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
auto BasicRegistry<IdType, ContainerTemplate>::at(Id t_id) -> Handle<Resource>
{
    return m_store.at<ContainerType<Resource>>().at(t_id);
}

template <typename IdType, template <typename...> typename ContainerTemplate>
template <typename Resource>
auto BasicRegistry<IdType, ContainerTemplate>::remove(Id t_id)
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

}   // namespace engine::asset
