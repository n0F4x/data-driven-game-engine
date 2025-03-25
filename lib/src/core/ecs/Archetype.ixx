module;

#include <algorithm>
#include <span>

export module core.ecs:Archetype;

import utility.meta.reflection.type_id;
import utility.TypeList;

import :component_c;
import :ComponentID;
import :sorted_component_id_sequence;

template <core::ecs::component_c... Components_T>
[[nodiscard]]
constexpr auto make_component_id_set() -> std::span<const ComponentID>
{
    using SortedComponentIDSequence =
        sorted_component_id_sequence_t<component_id<Components_T>.underlying()...>;

    constexpr static std::array<ComponentID, sizeof...(Components_T)> component_id_set =
        [] {
            std::array<ComponentID, sizeof...(Components_T)> result;

            SortedComponentIDSequence::enumerate(
                [&result]<size_t index_T, ComponentID::Underlying value_T> {
                    result[index_T] = ComponentID{ value_T };
                }
            );

            return result;
        }();

    return component_id_set;
}

class Archetype {
public:
    template <core::ecs::component_c... Components_T>
    consteval explicit Archetype(util::TypeList<Components_T...>)
        : m_id{ util::meta::id_v<
              sorted_component_id_sequence_t<component_id<Components_T>.underlying()...>> },
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
            ::sorted_component_id_sequence_t<(::component_id<Components_T>.underlying()
            )...>::realize(),
            {},
            {},
            [](const auto value) { return ComponentID{ value }; }
        );
    }

    template <core::ecs::component_c... Components_T>
    [[nodiscard]]
    constexpr auto contains_none_of_components() const noexcept -> bool
    {
        return util::TypeList<Components_T...>::for_each(
            [this]<typename Component_T> noexcept {
                // TODO: write TypeList adaptor `none_of`
                return std::ranges::
                    none_of(m_sorted_component_ids, component_id<Component_T>);
            }
        );
    }

private:
    friend struct ArchetypeInfoHashAdaptorClosure;

    util::meta::TypeID           m_id;
    std::span<const ComponentID> m_sorted_component_ids;
};

struct ArchetypeInfoHashAdaptorClosure {
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
        return ArchetypeInfoHashAdaptorClosure::operator()(archetype);
    }
};

template <core::ecs::component_c... Components_T>
[[nodiscard]]
constexpr auto archetype_from() noexcept -> const Archetype&
{
    constexpr static Archetype archetype{ util::TypeList<Components_T...>{} };

    return archetype;
}
