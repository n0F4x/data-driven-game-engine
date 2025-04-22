module;

#include <algorithm>
#include <concepts>
#include <functional>
#include <ranges>
#include <tuple>
#include <type_traits>

export module core.ecs:query.QueryClosure;

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

template <core::ecs::query_parameter_c T>
struct IsComponentOrIDOrOptional {
    constexpr static bool value = core::ecs::queryable_component_c<std::remove_const_t<T>>
                               || std::same_as<T, core::ecs::ID>
                               || util::meta::specialization_of_c<T, core::ecs::Optional>;
};

template <core::ecs::query_parameter_c T>
struct IsComponentOrWith {
    constexpr static bool value = core::ecs::queryable_component_c<std::remove_const_t<T>>
                               || util::meta::specialization_of_c<T, core::ecs::With>;
};

template <core::ecs::query_parameter_c T>
struct IsComponentOrWithOrOptional {
    constexpr static bool value = core::ecs::queryable_component_c<std::remove_const_t<T>>
                               || util::meta::specialization_of_c<T, core::ecs::With>
                               || util::meta::specialization_of_c<T, core::ecs::Optional>;
};

template <core::ecs::query_parameter_c T>
struct IsWithout {
    constexpr static bool value = util::meta::specialization_of_c<T, core::ecs::Without>;
};

template <core::ecs::query_parameter_c T>
struct IsQueriedParameter : IsComponentOrIDOrOptional<T> {};

template <core::ecs::query_parameter_c T>
struct ToQueriedType;

template <core::ecs::query_parameter_c T>
    requires core::ecs::queryable_component_c<std::remove_const_t<T>>
struct ToQueriedType<T> : std::type_identity<T> {};

template <core::ecs::query_parameter_c T>
    requires std::same_as<T, core::ecs::ID>
struct ToQueriedType<T> : std::type_identity<T> {};

template <core::ecs::query_parameter_c T>
    requires util::meta::specialization_of_c<T, core::ecs::Optional>
struct ToQueriedType<T> : std::type_identity<util::meta::underlying_t<T>> {};

template <core::ecs::query_parameter_c T>
struct ToFunctionParameter;

template <core::ecs::query_parameter_c T>
    requires core::ecs::queryable_component_c<std::remove_const_t<T>>
struct ToFunctionParameter<T> : std::type_identity<std::add_lvalue_reference_t<T>> {};

template <core::ecs::query_parameter_c T>
    requires std::same_as<T, core::ecs::ID>
struct ToFunctionParameter<T> : std::type_identity<T> {};

template <core::ecs::query_parameter_c T>
    requires util::meta::specialization_of_c<T, core::ecs::Optional>
struct ToFunctionParameter<T> : std::type_identity<T> {};

template <typename, typename>
struct invocable_with_impl;

template <typename F, template <typename...> typename TypeList_T, typename... FunctionParameters_T>
struct invocable_with_impl<F, TypeList_T<FunctionParameters_T...>>
    : std::bool_constant<std::invocable<F, FunctionParameters_T...>> {};

template <typename F, typename FunctionParametersTypeList_T>
concept invocable_with_c = invocable_with_impl<F, FunctionParametersTypeList_T>::value;

template <typename Component_T>
    requires core::ecs::component_c<std::remove_const_t<Component_T>>
struct ToOptionalComponentTableRef {
    using type = util::OptionalRef<
        util::meta::
            const_like_t<ComponentTable<std::remove_const_t<Component_T>>, Component_T>>;
};

template <typename Component_T>
using OptionalComponentTableRef = typename ToOptionalComponentTableRef<Component_T>::type;

template <typename Component_T>
    requires core::ecs::component_c<std::remove_const_t<Component_T>>
struct ToComponentContainer {
    using type = util::meta::
        const_like_t<ComponentContainer<std::remove_const_t<Component_T>>, Component_T>;
};

template <typename Component_T>
    requires core::ecs::component_c<std::remove_const_t<Component_T>>
struct ToComponentContainerRef {
    using type = ToComponentContainer<Component_T>&;
};

template <typename Component_T>
using ComponentContainerRef = typename ToComponentContainerRef<Component_T>::type;

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
struct QueryClosure {
private:
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
    template <::invocable_with_c<FunctionParameters> F>
    static auto operator()(core::ecs::Registry& registry, F&& func) -> F;

    [[nodiscard]]
    constexpr static auto matches_archetype(const Archetype& archetype) -> bool;

private:
    [[nodiscard]]
    static auto fill_included_optional_component_table_refs(
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        core::ecs::Registry&                registry
    ) -> bool;

    [[nodiscard]]
    static auto smallest_required_component_table_archetype_ids_from(
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs
    ) -> std::span<const ArchetypeID>;

    [[nodiscard]]
    static auto matching_archetype_ids_from(std::span<const ArchetypeID> archetype_ids);

    template <typename F>
    static auto visit_archetype(
        core::ecs::Registry&                registry,
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        ArchetypeID                         archetype_id,
        F                                   func
    ) -> void;

    template <typename QueriedParameter_T>
        requires(std::is_same_v<QueriedParameter_T, core::ecs::ID>)
    [[nodiscard]]
    static auto queried_type_view_from(
        core::ecs::Registry&                registry,
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        ArchetypeID                         archetype_id
    ) -> std::span<const core::ecs::ID>;

    template <typename QueriedParameter_T>
        requires(util::meta::specialization_of_c<QueriedParameter_T, core::ecs::Optional>)
    [[nodiscard]]
    static auto queried_type_view_from(
        core::ecs::Registry&                registry,
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        ArchetypeID                         archetype_id
    ) -> OptionalView<typename ToComponent<QueriedParameter_T>::type>;

    template <typename QueriedParameter_T>
        requires core::ecs::queryable_component_c<std::remove_const_t<QueriedParameter_T>>
    [[nodiscard]]
    static auto queried_type_view_from(
        core::ecs::Registry&                registry,
        IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
        ArchetypeID                         archetype_id
    )
        -> std::ranges::ref_view<typename ToComponentContainer<
            typename ToComponent<QueriedParameter_T>::type>::type>;
};

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <::invocable_with_c<util::meta::type_list_transform_t<
    util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsQueriedParameter>,
    ToFunctionParameter>> F>
auto QueryClosure<Parameters_T...>::operator()(core::ecs::Registry& registry, F&& func)
    -> F
{
    IncludedOptionalComponentTableRefs included_optional_component_table_refs{};

    if (!fill_included_optional_component_table_refs(
            included_optional_component_table_refs, registry
        ))
    {
        return std::forward<F>(func);
    };

    for (const ArchetypeID archetype_id :
         matching_archetype_ids_from(smallest_required_component_table_archetype_ids_from(
             included_optional_component_table_refs
         )))
    {
        visit_archetype(
            registry, included_optional_component_table_refs, archetype_id, func
        );
    }

    return std::forward<F>(func);
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
constexpr auto QueryClosure<Parameters_T...>::matches_archetype(const Archetype& archetype)
    -> bool
{
    return util::meta::apply<RequiredComponents>([&archetype]<typename... Ts> {
               return archetype.contains_all_of_components<std::remove_const_t<Ts>...>();
           })
        && util::meta::apply<ExcludedComponents>([&archetype]<typename... Ts> {
               return archetype.contains_none_of_components<std::remove_const_t<Ts>...>();
           });
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto QueryClosure<Parameters_T...>::fill_included_optional_component_table_refs(
    IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
    core::ecs::Registry&                registry
) -> bool
{
    if (util::meta::any_of<IncludedComponents>(
            [&registry,
             &included_optional_component_table_refs]<typename Component_T> mutable {
                OptionalComponentTableRef<Component_T>& optional_component_table_ref{
                    std::get<OptionalComponentTableRef<Component_T>>(
                        included_optional_component_table_refs
                    )
                };

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
    };

    return true;
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto QueryClosure<Parameters_T...>::smallest_required_component_table_archetype_ids_from(
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

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto QueryClosure<Parameters_T...>::matching_archetype_ids_from(
    std::span<const ArchetypeID> archetype_ids
)
{
    return archetype_ids | std::views::transform(&ArchetypeID::get)
         | std::views::filter(matches_archetype)
         | std::views::transform([](const Archetype& archetype) {
               return ArchetypeID{ archetype };
           });
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <typename F>
auto QueryClosure<Parameters_T...>::visit_archetype(
    core::ecs::Registry&                registry,
    IncludedOptionalComponentTableRefs& included_optional_component_table_refs,
    const ArchetypeID                   archetype_id,
    F                                   func
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

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <typename QueriedParameter_T>
    requires(std::is_same_v<QueriedParameter_T, core::ecs::ID>)
auto QueryClosure<Parameters_T...>::queried_type_view_from(
    core::ecs::Registry& registry,
    IncludedOptionalComponentTableRefs&,
    const ArchetypeID archetype_id
) -> std::span<const core::ecs::ID>
{
    return registry.m_lookup_tables.get_iterator(archetype_id)->second.ids();
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <typename QueriedParameter_T>
    requires(util::meta::specialization_of_c<QueriedParameter_T, core::ecs::Optional>)
auto QueryClosure<Parameters_T...>::queried_type_view_from(
    core::ecs::Registry&,
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

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <typename QueriedParameter_T>
    requires core::ecs::queryable_component_c<std::remove_const_t<QueriedParameter_T>>
auto QueryClosure<Parameters_T...>::queried_type_view_from(
    core::ecs::Registry&,
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
