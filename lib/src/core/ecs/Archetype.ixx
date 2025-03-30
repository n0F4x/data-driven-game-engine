module;

#include <algorithm>
#include <span>

export module core.ecs:Archetype;

import utility.meta.reflection.hash;
import utility.meta.type_traits.integer_sequence.integer_sequence_sort;
import utility.TypeList;
import utility.ValueSequence;

import :component_c;
import :ComponentID;

template <core::ecs::component_c... Components_T>
[[nodiscard]]
constexpr auto make_component_id_set() -> std::span<const ComponentID>
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

class Archetype {
public:
    template <core::ecs::component_c... Components_T>
    consteval explicit Archetype(util::TypeList<Components_T...>)
        : m_id{ util::meta::hash<util::meta::integer_sequence_sort_t<util::ValueSequence<
              ComponentID::Underlying,
              component_id_of<Components_T>().underlying()...>>> },
          m_sorted_component_ids{ make_component_id_set<Components_T...>() }
    {}

    constexpr auto operator==(const Archetype& other) const noexcept -> bool
    {
        return m_id == other.m_id;
    }

    constexpr auto operator<=>(const Archetype& other) const noexcept
    {
        return m_id <=> other.m_id;
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
            make_component_id_set<Components_T...>(),
            {},
            {},
            [](const auto value) { return ComponentID{ value }; }
        );
    }

    template <core::ecs::component_c... Components_T>
    [[nodiscard]]
    constexpr auto contains_none_of_components() const noexcept -> bool
    {
        return (
            (!std::ranges::
                 binary_search(m_sorted_component_ids, component_id_of<Components_T>()))
            && ...
        );
    }

private:
    friend struct ArchetypeInfoHashClosure;

    util::meta::TypeHash         m_id;
    std::span<const ComponentID> m_sorted_component_ids;
};

struct ArchetypeInfoHashClosure {
    [[nodiscard]]
    constexpr static auto operator()(const Archetype& archetype) noexcept -> size_t
    {
        return archetype.m_id;
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
    constexpr static Archetype value{ util::TypeList<Components_T...>{} };

    consteval static auto operator()() -> const Archetype&
    {
        return value;
    }
};

template <core::ecs::component_c... Components_T>
constexpr inline ArchetypeFromClosure<Components_T...> archetype_from;
