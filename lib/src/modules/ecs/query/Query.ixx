module;

#include <algorithm>
#include <concepts>
#include <functional>
#include <ranges>
#include <tuple>
#include <type_traits>

#include "utility/lifetime_bound.hpp"

export module modules.ecs:query.QueryClosure;

import utility.meta.algorithms.apply;
import utility.meta.algorithms.any_of;
import utility.meta.algorithms.fold_left_first;
import utility.containers.OptionalRef;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.type_list.type_list_size;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.underlying;
import utility.TypeList;

import :ComponentTable;
import :ComponentTableMap.extensions;
import :ID;
import :query.OptionalView;
import :query.queryable_component_c;
import :query.ToComponent;
import :Registry;

template <modules::ecs::query_parameter_c T>
struct IsComponentOrIDOrOptional {
    constexpr static bool value = modules::ecs::queryable_component_c<std::remove_const_t<T>>
                               || std::same_as<T, modules::ecs::ID>
                               || util::meta::specialization_of_c<T, modules::ecs::Optional>;
};

template <modules::ecs::query_parameter_c T>
struct IsComponentOrWith {
    constexpr static bool value = modules::ecs::queryable_component_c<std::remove_const_t<T>>
                               || util::meta::specialization_of_c<T, modules::ecs::With>;
};

template <modules::ecs::query_parameter_c T>
struct IsComponentOrWithOrOptional {
    constexpr static bool value = modules::ecs::queryable_component_c<std::remove_const_t<T>>
                               || util::meta::specialization_of_c<T, modules::ecs::With>
                               || util::meta::specialization_of_c<T, modules::ecs::Optional>;
};

template <modules::ecs::query_parameter_c T>
struct IsWithout {
    constexpr static bool value = util::meta::specialization_of_c<T, modules::ecs::Without>;
};

template <modules::ecs::query_parameter_c T>
struct IsQueriedParameter : IsComponentOrIDOrOptional<T> {};

template <modules::ecs::query_parameter_c T>
struct ToQueriedType;

template <modules::ecs::query_parameter_c T>
    requires modules::ecs::queryable_component_c<std::remove_const_t<T>>
struct ToQueriedType<T> : std::type_identity<T> {};

template <modules::ecs::query_parameter_c T>
    requires std::same_as<T, modules::ecs::ID>
struct ToQueriedType<T> : std::type_identity<T> {};

template <modules::ecs::query_parameter_c T>
    requires util::meta::specialization_of_c<T, modules::ecs::Optional>
struct ToQueriedType<T> : std::type_identity<util::meta::underlying_t<T>> {};

template <modules::ecs::query_parameter_c T>
struct ToFunctionParameter;

template <modules::ecs::query_parameter_c T>
    requires modules::ecs::queryable_component_c<std::remove_const_t<T>>
struct ToFunctionParameter<T> : std::type_identity<std::add_lvalue_reference_t<T>> {};

template <modules::ecs::query_parameter_c T>
    requires std::same_as<T, modules::ecs::ID>
struct ToFunctionParameter<T> : std::type_identity<T> {};

template <modules::ecs::query_parameter_c T>
    requires util::meta::specialization_of_c<T, modules::ecs::Optional>
struct ToFunctionParameter<T> : std::type_identity<T> {};

template <typename, typename>
struct invocable_with_impl;

template <typename F, template <typename...> typename TypeList_T, typename... FunctionParameters_T>
struct invocable_with_impl<F, TypeList_T<FunctionParameters_T...>>
    : std::bool_constant<std::invocable<F, FunctionParameters_T...>> {};

template <typename F, typename FunctionParametersTypeList_T>
concept invocable_with_c = invocable_with_impl<F, FunctionParametersTypeList_T>::value;

template <typename Component_T>
    requires modules::ecs::component_c<std::remove_const_t<Component_T>>
struct ToOptionalComponentTableRef {
    using type = util::OptionalRef<
        util::meta::
            const_like_t<ComponentTable<std::remove_const_t<Component_T>>, Component_T>>;
};

template <typename Component_T>
using OptionalComponentTableRef = typename ToOptionalComponentTableRef<Component_T>::type;

template <typename Component_T>
    requires modules::ecs::component_c<std::remove_const_t<Component_T>>
struct ToComponentContainer {
    using type = util::meta::
        const_like_t<ComponentContainer<std::remove_const_t<Component_T>>, Component_T>;
};

template <typename Component_T>
    requires modules::ecs::component_c<std::remove_const_t<Component_T>>
struct ToComponentContainerRef {
    using type = ToComponentContainer<Component_T>&;
};

template <typename Component_T>
using ComponentContainerRef = typename ToComponentContainerRef<Component_T>::type;

namespace modules::ecs {

export template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
class Query {
    using QueriedParameters =
        util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsQueriedParameter>;

    static_assert(
        util::meta::type_list_size_v<QueriedParameters> != 0,
        "query must have actually queried types"
    );

    using QueriedTypes =
        util::meta::type_list_transform_t<QueriedParameters, ToQueriedType>;

    using FunctionParameters =
        util::meta::type_list_transform_t<QueriedParameters, ToFunctionParameter>;

    using IncludedComponents = util::meta::type_list_transform_t<
        util::meta::
            type_list_filter_t<util::TypeList<Parameters_T...>, IsComponentOrWithOrOptional>,
        ToComponent>;

    using RequiredComponents = util::meta::type_list_transform_t<
        util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsComponentOrWith>,
        ToComponent>;

    static_assert(
        util::meta::type_list_size_v<RequiredComponents> != 0,
        "query must have required components"
    );

    using ExcludedComponents = util::meta::type_list_transform_t<
        util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsWithout>,
        ToComponent>;

    using IncludedOptionalComponentTableRefs = util::meta::type_list_to_t<
        util::meta::type_list_transform_t<IncludedComponents, ToOptionalComponentTableRef>,
        std::tuple>;

public:
    [[nodiscard]]
    constexpr static auto matches_archetype(const Archetype& archetype) -> bool;

    explicit Query(modules::ecs::Registry& registry [[lifetime_bound]]);

    template <::invocable_with_c<FunctionParameters> F>
    auto operator()(F&& func) -> F;

    [[nodiscard]]
    auto count() -> std::size_t;

private:
    [[nodiscard]]
    static auto fill_included_optional_component_table_refs(
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        modules::ecs::Registry&                registry
    ) -> bool;

    [[nodiscard]]
    static auto smallest_group_of_required_archetype_ids_from(
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs
    ) -> std::span<const ArchetypeID>;

    [[nodiscard]]
    static auto matching_archetype_ids_from(std::span<const ArchetypeID> archetype_ids);

    template <typename F>
    static auto visit_archetype(
        modules::ecs::Registry&                registry,
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        ArchetypeID                         archetype_id,
        F&&                                 func
    ) -> void;

    template <typename QueriedParameter_T>
        requires(std::is_same_v<QueriedParameter_T, modules::ecs::ID>)
    [[nodiscard]]
    static auto queried_type_view_from(
        modules::ecs::Registry&                registry,
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        ArchetypeID                         archetype_id
    ) -> std::span<const modules::ecs::ID>;

    template <typename QueriedParameter_T>
        requires(util::meta::specialization_of_c<QueriedParameter_T, modules::ecs::Optional>)
    [[nodiscard]]
    static auto queried_type_view_from(
        modules::ecs::Registry&                registry,
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        ArchetypeID                         archetype_id
    ) -> OptionalView<typename ToComponent<QueriedParameter_T>::type>;

    template <typename QueriedParameter_T>
        requires modules::ecs::queryable_component_c<std::remove_const_t<QueriedParameter_T>>
    [[nodiscard]]
    static auto queried_type_view_from(
        modules::ecs::Registry&                registry,
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        ArchetypeID                         archetype_id
    )
        -> std::ranges::ref_view<typename ToComponentContainer<
            typename ToComponent<QueriedParameter_T>::type>::type>;

    std::reference_wrapper<Registry>   m_registry_ref;
    IncludedOptionalComponentTableRefs m_included_optional_component_table_refs;
    bool                               m_acquired_all_included_component_tables{};

    auto cache_component_tables() -> void;
};

}   // namespace modules::ecs

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
constexpr auto modules::ecs::Query<Parameters_T...>::matches_archetype(
    const Archetype& archetype
) -> bool
{
    return util::meta::apply<RequiredComponents>([&archetype]<typename... Ts> {
               return archetype.contains_all_of_components<std::remove_const_t<Ts>...>();
           })
        && util::meta::apply<ExcludedComponents>([&archetype]<typename... Ts> {
               return archetype.contains_none_of_components<std::remove_const_t<Ts>...>();
           });
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
modules::ecs::Query<Parameters_T...>::Query(modules::ecs::Registry& registry)
    : m_registry_ref{ registry }
{}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <::invocable_with_c<util::meta::type_list_transform_t<
    util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsQueriedParameter>,
    ToFunctionParameter>> F>
auto modules::ecs::Query<Parameters_T...>::operator()(F&& func) -> F
{
    cache_component_tables();
    if (!m_acquired_all_included_component_tables) {
        return std::forward<F>(func);
    }

    for (const ArchetypeID archetype_id :
         matching_archetype_ids_from(smallest_group_of_required_archetype_ids_from(
             m_included_optional_component_table_refs
         )))
    {
        visit_archetype(
            m_registry_ref, m_included_optional_component_table_refs, archetype_id, func
        );
    }

    return std::forward<F>(func);
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto modules::ecs::Query<Parameters_T...>::count() -> std::size_t
{
    cache_component_tables();
    if (!m_acquired_all_included_component_tables) {
        return 0;
    }

    std::size_t result{};
    for (const ArchetypeID archetype_id :
         matching_archetype_ids_from(smallest_group_of_required_archetype_ids_from(
             m_included_optional_component_table_refs
         )))
    {
        util::meta::apply<QueriedParameters>(
            [this, &result, archetype_id]<typename... QueriedParameter_T> {
                result += std::views::zip(
                              queried_type_view_from<QueriedParameter_T>(
                                  m_registry_ref,
                                  m_included_optional_component_table_refs,
                                  archetype_id
                              )...
                )
                              .size();
            }
        );
    }
    return result;
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto modules::ecs::Query<Parameters_T...>::fill_included_optional_component_table_refs(
    IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
    modules::ecs::Registry&                registry
) -> bool
{
    if (util::meta::any_of<IncludedComponents>(
            [&registry, &included_optional_component_table_refs]<typename Component_T> {
                OptionalComponentTableRef<Component_T>& optional_component_table_ref{
                    std::get<OptionalComponentTableRef<Component_T>>(
                        included_optional_component_table_refs
                    )
                };

                if (optional_component_table_ref.has_value()) {
                    return false;
                }

                optional_component_table_ref =
                    find_component_table<std::remove_const_t<Component_T>>(
                        registry.m_component_tables
                    );

                return util::meta::type_list_contains_v<RequiredComponents, Component_T>
                    && !optional_component_table_ref.has_value();
            }
        ))
    {
        return false;
    }

    return true;
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto modules::ecs::Query<Parameters_T...>::smallest_group_of_required_archetype_ids_from(
    IncludedOptionalComponentTableRefs& included_optional_component_table_refs
) -> std::span<const ArchetypeID>
{
    return util::meta::fold_left_first<RequiredComponents>(
        [&included_optional_component_table_refs]<typename Component_T> {
            return std::get<OptionalComponentTableRef<Component_T>>(
                       included_optional_component_table_refs
            )
                ->archetype_ids();
        },
        [](std::span<const ArchetypeID> lhs, std::span<const ArchetypeID> rhs) {
            if (lhs.size() < rhs.size()) {
                return lhs;
            }
            return rhs;
        }
    );
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto modules::ecs::Query<Parameters_T...>::matching_archetype_ids_from(
    std::span<const ArchetypeID> archetype_ids
)
{
    return archetype_ids | std::views::transform(&ArchetypeID::get)
         | std::views::filter(matches_archetype)
         | std::views::transform([](const Archetype& archetype) {
               return ArchetypeID{ archetype };
           });
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <typename F>
auto modules::ecs::Query<Parameters_T...>::visit_archetype(
    modules::ecs::Registry&                registry,
    IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
    const ArchetypeID                   archetype_id,
    F&&                                 func
) -> void
{
    // TODO: Optional ranges produce a view that always checks availability
    //       Check it only once!

    util::meta::apply<QueriedParameters>([&,
                                          archetype_id]<typename... QueriedParameter_T> {
        std::ranges::for_each(
            std::views::zip(
                queried_type_view_from<QueriedParameter_T>(
                    registry, included_optional_component_table_refs, archetype_id
                )...
            ),
            [&func](auto queried_type_tuple) { std::apply(func, queried_type_tuple); }
        );
    });
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <typename QueriedParameter_T>
    requires(std::is_same_v<QueriedParameter_T, modules::ecs::ID>)
auto modules::ecs::Query<Parameters_T...>::queried_type_view_from(
    modules::ecs::Registry& registry,
    IncludedOptionalComponentTableRefs&,
    const ArchetypeID archetype_id
) -> std::span<const modules::ecs::ID>
{
    return registry.m_lookup_tables.get_iterator(archetype_id)->second.ids();
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <typename QueriedParameter_T>
    requires(util::meta::specialization_of_c<QueriedParameter_T, modules::ecs::Optional>)
auto modules::ecs::Query<Parameters_T...>::queried_type_view_from(
    modules::ecs::Registry&,
    IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
    const ArchetypeID                   archetype_id
) -> OptionalView<typename ToComponent<QueriedParameter_T>::type>
{
    const auto component_table_ptr{
        std::get<OptionalComponentTableRef<typename ToComponent<QueriedParameter_T>::type>>(
            included_optional_component_table_refs
        )
    };
    if (!component_table_ptr.has_value()) {
        return OptionalView<typename ToComponent<QueriedParameter_T>::type>{};
    }

    const util::OptionalRef optional_component_container{
        component_table_ptr->find_component_container(archetype_id)
    };
    if (!optional_component_container.has_value()) {
        return OptionalView<typename ToComponent<QueriedParameter_T>::type>{};
    }

    return OptionalView<typename ToComponent<QueriedParameter_T>::type>{
        *optional_component_container
    };
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <typename QueriedParameter_T>
    requires modules::ecs::queryable_component_c<std::remove_const_t<QueriedParameter_T>>
auto modules::ecs::Query<Parameters_T...>::queried_type_view_from(
    modules::ecs::Registry&,
    IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
    const ArchetypeID                   archetype_id
)
    -> std::ranges::ref_view<
        typename ToComponentContainer<typename ToComponent<QueriedParameter_T>::type>::type>
{
    static typename ToComponentContainer<typename ToComponent<QueriedParameter_T>::type>::
        type empty_container;

    return std::
        get<OptionalComponentTableRef<typename ToComponent<QueriedParameter_T>::type>>(
               included_optional_component_table_refs
        )
            ->find_component_container(archetype_id)
            .transform([](auto& component_container) {
                return std::ranges::ref_view{ component_container };
            })
            .value_or(std::ranges::ref_view{ empty_container });
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto modules::ecs::Query<Parameters_T...>::cache_component_tables() -> void
{
    if (m_acquired_all_included_component_tables) {
        return;
    }

    m_acquired_all_included_component_tables = fill_included_optional_component_table_refs(
        m_included_optional_component_table_refs, m_registry_ref
    );
}
