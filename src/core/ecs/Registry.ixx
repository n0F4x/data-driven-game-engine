module;

#include <any>
#include <cstdint>
#include <set>
#include <tuple>
#include <unordered_map>

#ifdef ENGINE_ENABLE_TESTS
  #include <catch2/catch_test_macros.hpp>
#endif

export module core.ecs.Registry;

import utility.containers.Any;
import utility.containers.MultiSparseSet;
import utility.containers.SparseSet;
import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
import utility.meta.reflection.hash;
import utility.meta.type_traits.all_different;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_is_sorted;
import utility.meta.type_traits.type_list.type_list_sort;
import utility.meta.type_traits.type_list.type_list_to;
import utility.all_same;
import utility.hashing;
import utility.ScopeGuard;
import utility.TypeList;

namespace core::ecs {

using ID = uint32_t;

template <typename T>
concept component_c = !std::is_const_v<T> && util::meta::nothrow_movable_c<T>;

template <typename T>
concept decays_to_component_c = component_c<std::decay_t<T>>;

using ComponentID = util::meta::hash_type;

template <typename Component_T>
struct component_id {
    constexpr static ComponentID value =
        util::meta::hash_v<std::remove_cvref_t<Component_T>>;
};

template <typename Component_T>
constexpr ComponentID component_id_v = component_id<Component_T>::value;

using ArchetypeID = util::meta::hash_type;

template <typename... Components_T>
    requires(util::meta::
                 type_list_is_sorted_v<util::TypeList<Components_T...>, component_id>)
class BasicArchetype {
public:
    util::MultiSparseSet<ID, util::TypeList<Components_T...>> component_containers;
};

template <typename... Components_T>
constexpr bool archetype_id_v = util::meta::hash_v<BasicArchetype<Components_T...>>;

class ErasedArchetype : public util::Any {
    using Base = util::Any;

public:
    template <typename Archetype_T, typename... Args_T>
        requires util::meta::
                     specialization_of_c<std::remove_cvref_t<Archetype_T>, BasicArchetype>
    constexpr explicit ErasedArchetype(std::in_place_type_t<Archetype_T>, Args_T&&... args)
        : Base{ std::in_place_type<Archetype_T>, std::forward<Args_T>(args)... },
          m_erase_func{ make_erase_func<Archetype_T>() }
    {}

    template <typename Archetype_T>
        requires util::meta::
            specialization_of_c<std::remove_cvref_t<Archetype_T>, BasicArchetype>
        constexpr explicit ErasedArchetype(Archetype_T&& archetype)
        : ErasedArchetype{ std::in_place_type<std::remove_cvref_t<Archetype_T>>,
                           std::forward<Archetype_T>(archetype) }
    {}

    // TODO: assignment

    constexpr auto erase(const ID id) -> bool
    {
        return m_erase_func(*this, id);
    }

private:
    using EraseFunc = bool (*)(Base&, ID);

    EraseFunc m_erase_func;

    template <typename Archetype_T>
    [[nodiscard]]
    constexpr static auto make_erase_func() -> EraseFunc
    {
        return +[](Base& base, const ID id) {
            return util::any_cast<std::remove_cvref_t<Archetype_T>&>(base)
                .component_containers.erase(id)
                .has_value();
        };
    }
};

struct Entity {
    ArchetypeID archetype_id;
    ID          id_within_archetype;
};

// TODO: constexpr when std::unordered_map becomes constexpr
// TODO: remove emptied archetypes (this also means more exception guarantees upon create)
class Registry {
public:
    template <decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0, util::meta::all_different_v<std::decay_t<Components_T>...>)
    auto create(Components_T&&... components) -> ID;

    auto destroy(ID entity_id) -> bool;

private:
    std::unordered_map<ArchetypeID, ErasedArchetype> m_archetypes;
    std::unordered_map<ComponentID, std::set<ArchetypeID>> m_component_to_archetype_mapping;
    util::SparseSet<ID, Entity> m_entities;
};

}   // namespace core::ecs

template <core::ecs::decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0, util::meta::all_different_v<std::decay_t<Components_T>...>)
auto core::ecs::Registry::create(Components_T&&... components) -> ID
{
    using Archetype = util::meta::type_list_to_t<
        util::meta::type_list_sort_t<util::TypeList<Components_T...>, component_id>,
        BasicArchetype>;

    constexpr static ArchetypeID archetype_id{ archetype_id_v<Archetype> };

    Archetype& archetype{ util::any_cast<Archetype&>(
        m_archetypes.try_emplace(archetype_id, std::in_place_type<Archetype>).first->second
    ) };

    const ID id_within_archetype =
        archetype.component_containers.emplace(std::forward<Components_T>(components)...);
    ::util::ScopeGuard component_guard{
        ::util::make_scope_guard([&archetype, id_within_archetype] noexcept {
            archetype.component_containers.erase(id_within_archetype);
        })
    };

    (m_component_to_archetype_mapping[component_id_v<Components_T>].insert(archetype_id),
     ...);

    return m_entities.emplace(
        Entity{ .archetype_id = archetype_id, .id_within_archetype = id_within_archetype }
    );
}

auto core::ecs::Registry::destroy(const ID entity_id) -> bool
{
    return m_entities.erase(entity_id)
        .transform([this](const Entity entity) {
            ErasedArchetype& archetype{ m_archetypes.at(entity.archetype_id) };
            const bool       success = archetype.erase(entity.id_within_archetype);
            assert(success);

            return true;
        })
        .value_or(false);
}

module :private;

#ifdef ENGINE_ENABLE_TESTS

TEST_CASE("core::ecs::Registry")
{
    core::ecs::Registry registry;

    SECTION("create")
    {
        decltype(auto) id = registry.create(int{}, float{});

        static_assert(std::is_same_v<decltype(id), core::ecs::ID>);
    }

    SECTION("destroy contained")
    {
        const auto id      = registry.create(int{}, float{});
        const bool success = registry.destroy(id);

        REQUIRE(success);
    }

    SECTION("destroy non-contained")
    {
        const bool success = registry.destroy(core::ecs::ID{});

        REQUIRE(!success);
    }
}

#endif
