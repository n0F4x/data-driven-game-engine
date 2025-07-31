module;

#include <algorithm>
#include <deque>
#include <ranges>
#include <span>
#include <vector>

export module core.ecs:Archetype;

import utility.meta.algorithms.none_of;
import utility.meta.concepts.ranges.input_range_of;
import utility.meta.reflection.hash;
import utility.meta.type_traits.integer_sequence.integer_sequence_sort;
import utility.hashing;
import utility.TypeList;
import utility.ValueSequence;

import :component_c;
import :ComponentID;

template <core::ecs::component_c... Components_T>
constexpr auto component_id_set_from_impl = [] {
    std::array<ComponentID, sizeof...(Components_T)> result{
        component_id_of<Components_T>()...
    };

    std::ranges::sort(result);

    return result;
}();

template <core::ecs::component_c... Components_T>
[[nodiscard]]
consteval auto component_id_set_from() -> std::span<const ComponentID>
{
    return component_id_set_from_impl<Components_T...>;
}

template <
    core::ecs::component_c... Components_T,
    util::meta::input_range_of_c<ComponentID> ComponentIDSet_T>
[[nodiscard]]
constexpr auto component_id_set_from(ComponentIDSet_T component_id_set)
    -> std::vector<ComponentID>
{
    constexpr std::span<const ComponentID> base_component_ids{
        component_id_set_from<Components_T...>()
    };

    std::vector<ComponentID> result;

    if constexpr (std::ranges::sized_range<ComponentIDSet_T>) {
        result.reserve(base_component_ids.size() + std::ranges::size(component_id_set));
    }
    else {
        result.reserve(base_component_ids.size());
    }

    std::ranges::set_union(
        base_component_ids, component_id_set, std::back_inserter(result)
    );

    return result;
}

class Archetype {
public:
    template <core::ecs::component_c... Components_T>
    constexpr explicit Archetype(util::TypeList<Components_T...>)
        : Archetype{ util::TypeList<Components_T...>{}, std::views::empty<ComponentID> }
    {}

    template <
        core::ecs::component_c... Components_T,
        util::meta::input_range_of_c<ComponentID> ComponentIDSet_T>
    constexpr explicit Archetype(
        util::TypeList<Components_T...>,
        ComponentIDSet_T component_id_set
    )
        : m_component_id_set{ component_id_set_from<Components_T...>(component_id_set) }
    {}

    constexpr auto operator==(const Archetype& other) const noexcept -> bool
    {
        return std::ranges::equal(m_component_id_set, other.m_component_id_set);
    }

    auto operator<=>(const Archetype& other) const = default;

    [[nodiscard]]
    constexpr auto sorted_component_ids() const noexcept -> std::span<const ComponentID>
    {
        return m_component_id_set;
    }

    template <core::ecs::component_c... Components_T>
    [[nodiscard]]
    constexpr auto contains_all_of_components() const noexcept -> bool
    {
        return std::ranges::includes(
            m_component_id_set,
            component_id_set_from<Components_T...>(),
            {},
            {},
            [](const auto value) { return ComponentID{ value }; }
        );
    }

    template <core::ecs::component_c... Components_T>
    [[nodiscard]]
    constexpr auto contains_none_of_components() const noexcept -> bool
    {
        return util::meta::none_of<util::TypeList<Components_T...>>(
            [this]<typename Component_T> {
                return std::ranges::binary_search(
                    m_component_id_set, component_id_of<Component_T>()
                );
            }
        );
    }

private:
    friend struct ArchetypeInfoHashClosure;

    std::vector<ComponentID> m_component_id_set;
};

struct ArchetypeInfoHashClosure {
    [[nodiscard]]
    constexpr static auto operator()(const Archetype& archetype) noexcept -> std::size_t
    {
        return util::hash_range(archetype.m_component_id_set);
    }
};

template <>
struct std::hash<Archetype> {
    [[nodiscard]]
    constexpr static auto operator()(const Archetype& archetype) noexcept -> std::size_t
    {
        return ArchetypeInfoHashClosure::operator()(archetype);
    }
};

template <core::ecs::component_c... Components_T>
auto archetype_from() -> const Archetype&
{
    static const Archetype value{ util::TypeList<Components_T...>{} };
    return value;
}

template <
    core::ecs::component_c... Components_T,
    util::meta::input_range_of_c<ComponentID> ComponentIDSet_T>
auto archetype_from(ComponentIDSet_T component_id_set) -> const Archetype&
{
    static std::deque<Archetype> archetypes;

    Archetype new_archetype{ util::TypeList<Components_T...>{}, component_id_set };

    if (const auto iter{ std::ranges::find(archetypes, new_archetype) };
        iter != std::ranges::cend(archetypes))
    {
        return *iter;
    }

    archetypes.push_back(std::move(new_archetype));
    return archetypes.back();
}
