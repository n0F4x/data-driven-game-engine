module;

#include <algorithm>
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
[[nodiscard]]
constexpr auto component_id_set_from() -> std::span<const ComponentID>
{
    constexpr static std::array<ComponentID, sizeof...(Components_T)> component_id_set =
        [] {
            std::array<ComponentID, sizeof...(Components_T)> result{
                component_id_of<Components_T>()...
            };

            std::ranges::sort(result);

            return result;
        }();

    return component_id_set;
}

template <
    core::ecs::component_c... Components_T,
    util::meta::input_range_of_c<ComponentID> SortedComponentIDRange_T>
[[nodiscard]]
constexpr auto component_id_set_from(SortedComponentIDRange_T sorted_component_ids)
    -> std::vector<ComponentID>
{
    constexpr std::span<const ComponentID> base_component_ids{
        component_id_set_from<Components_T...>()
    };

    std::vector<ComponentID> result;

    if constexpr (std::ranges::sized_range<SortedComponentIDRange_T>) {
        result.reserve(
            base_component_ids.size() + std::ranges::size(sorted_component_ids)
        );
    }
    else {
        result.reserve(base_component_ids.size());
    }

    std::ranges::set_union(
        base_component_ids, sorted_component_ids, std::back_inserter(result)
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
        util::meta::input_range_of_c<ComponentID> SortedComponentIDRange_T>
    constexpr explicit Archetype(
        util::TypeList<Components_T...>,
        SortedComponentIDRange_T sorted_component_ids
    )
        : m_sorted_component_ids{
              component_id_set_from<Components_T...>(sorted_component_ids)
          }
    {}

    constexpr auto operator==(const Archetype& other) const noexcept -> bool
    {
        return std::ranges::equal(m_sorted_component_ids, other.m_sorted_component_ids);
    }

    constexpr auto operator<(const Archetype& other) const noexcept
    {
        return std::ranges::lexicographical_compare(
            m_sorted_component_ids, other.m_sorted_component_ids
        );
    }

    [[nodiscard]]
    constexpr auto sorted_component_ids() const noexcept -> std::span<const ComponentID>
    {
        return m_sorted_component_ids;
    }

    template <core::ecs::component_c... Components_T>
    [[nodiscard]]
    constexpr auto contains_components() const noexcept -> bool
    {
        return std::ranges::includes(
            m_sorted_component_ids,
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
                    m_sorted_component_ids, component_id_of<Component_T>()
                );
            }
        );
    }

private:
    friend struct ArchetypeInfoHashClosure;

    std::vector<ComponentID> m_sorted_component_ids;
};

struct ArchetypeInfoHashClosure {
    [[nodiscard]]
    constexpr static auto operator()(const Archetype& archetype) noexcept -> size_t
    {
        return util::hash_range(archetype.m_sorted_component_ids);
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
struct ArchetypeFromClosure {
    constexpr static auto operator()() -> const Archetype&
    {
        static const Archetype value{ util::TypeList<Components_T...>{} };
        return value;
    }

    template <util::meta::input_range_of_c<ComponentID> SortedComponentIDRange_T>
    constexpr static auto operator()(SortedComponentIDRange_T component_ids)
        -> const Archetype&
    {
        static const Archetype value{ util::TypeList<Components_T...>{}, component_ids };
        return value;
    }
};

template <core::ecs::component_c... Components_T>
constexpr inline ArchetypeFromClosure<Components_T...> archetype_from;
