module;

#include <optional>
#include <span>
#include <vector>

export module core.ecs.Archetype;

import utility.containers.Any;
import utility.containers.SparseSet;
import utility.meta.reflection.type_hash;
import utility.meta.type_traits.type_list.type_list_sort;
import utility.Strong;
import utility.TypeList;

import core.ecs.Entity;
import core.ecs.Component;

struct archetype_id_tag_t {};

export using ArchetypeID = ::util::Strong<util::meta::TypeHash, archetype_id_tag_t>;

template <core::ecs::component_c Component_T>
struct component_hash {
    constexpr static size_t value{ core::ecs::component_id_v<Component_T>.underlying() };
};

template <core::ecs::component_c... Components_T>
using sorted_component_list_t =
    util::meta::type_list_sort_t<util::TypeList<Components_T...>, component_hash>;

template <core::ecs::component_c... Components_T>
constexpr ArchetypeID archetype_id_v{
    util::meta::hash_v<sorted_component_list_t<Components_T...>>
};

class Archetype {
public:
    template <core::ecs::component_c... Components_T>
    constexpr explicit Archetype(util::TypeList<Components_T...>);

    [[nodiscard]]
    constexpr auto component_set() const -> std::span<const core::ecs::ComponentID>;

    constexpr auto emplace() -> std::pair<core::ecs::Key, core::ecs::Index>;

    constexpr auto erase(core::ecs::Key key) -> std::optional<core::ecs::Index>;

    [[nodiscard]]
    constexpr auto get(core::ecs::Key key) const -> core::ecs::Index;

    [[nodiscard]]
    constexpr auto find(core::ecs::Key key) const noexcept
        -> std::optional<core::ecs::Index>;

    [[nodiscard]]
    constexpr auto contains(core::ecs::Key key) const noexcept -> bool;

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;

private:
    std::vector<core::ecs::ComponentID> m_component_id_set;
    util::SparseSet<
        core::ecs::Key,
        (sizeof(core::ecs::Key::Underlying) - sizeof(core::ecs::Index::Underlying)) * 8>
        m_sparse_index_set;
};

template <core::ecs::component_c... Components_T>
[[nodiscard]]
constexpr auto make_component_id_set() -> std::vector<core::ecs::ComponentID>
{
    using SortedComponentList = sorted_component_list_t<Components_T...>;

    std::vector<core::ecs::ComponentID> result;
    result.reserve(sizeof...(Components_T));

    SortedComponentList::for_each([&result]<typename Component_T> {
        result.push_back(core::ecs::component_id_v<Component_T>);
    });

    return result;
}

template <core::ecs::component_c... Components_T>
constexpr Archetype::Archetype(util::TypeList<Components_T...>)
    : m_component_id_set{ make_component_id_set<Components_T...>() }
{}

constexpr auto Archetype::component_set() const -> std::span<const core::ecs::ComponentID>
{
    return m_component_id_set;
}

constexpr auto Archetype::emplace() -> std::pair<core::ecs::Key, core::ecs::Index>
{
    const auto [key, index] = m_sparse_index_set.emplace();
    return std::make_pair(core::ecs::Key{ key }, core::ecs::Index{ index });
}

constexpr auto Archetype::erase(const core::ecs::Key key)
    -> std::optional<core::ecs::Index>
{
    return m_sparse_index_set.erase(key).transform(core::ecs::make_index);
}

constexpr auto Archetype::get(const core::ecs::Key key) const -> core::ecs::Index
{
    return core::ecs::Index{ m_sparse_index_set.get(key) };
}

constexpr auto Archetype::find(const core::ecs::Key key) const noexcept
    -> std::optional<core::ecs::Index>
{
    return m_sparse_index_set.find(key).transform(core::ecs::make_index);
}

constexpr auto Archetype::contains(const core::ecs::Key key) const noexcept -> bool
{
    return m_sparse_index_set.contains(key);
}

constexpr auto Archetype::empty() const noexcept -> bool
{
    return m_sparse_index_set.empty();
}

namespace core::ecs {

export using ArchetypeID = ::ArchetypeID;

export template <core::ecs::component_c... Components_T>
constexpr ArchetypeID archetype_id_v = ::archetype_id_v<Components_T...>;

export using Archetype = ::Archetype;

}   // namespace core::ecs
