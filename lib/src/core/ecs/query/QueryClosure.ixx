module;

#include <algorithm>
#include <array>
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
import utility.meta.concepts.functional.callable;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_index_of;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.type_list.type_list_size;
import utility.meta.type_traits.underlying;
import utility.TypeList;

import :ComponentContainer;
import :ComponentTable;
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
struct ToQueriedType<T> : std::type_identity<std::remove_const_t<T>> {};

template <core::ecs::query_parameter_c T>
    requires std::same_as<T, core::ecs::ID>
struct ToQueriedType<T> : std::type_identity<T> {};

template <core::ecs::query_parameter_c T>
    requires util::meta::specialization_of_c<T, core::ecs::Optional>
struct ToQueriedType<T>
    : std::type_identity<std::remove_const_t<util::meta::underlying_t<T>>> {};

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

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
struct QueryClosure {
private:
    using QueriedParameters =
        util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsQueriedParameter>;

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

public:
    template <::invocable_with_c<FunctionParameters> F>
    static auto operator()(core::ecs::Registry& registry, F&& func) -> F;

    [[nodiscard]]
    constexpr static auto matches_archetype(const Archetype& archetype) -> bool;

private:
    template <core::ecs::component_c Component_T>
    [[nodiscard]]
    consteval static auto include_index_of() -> size_t;

    [[nodiscard]]
    static auto fill_included_component_table_pointers(
        std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
                             included_component_table_pointers,
        core::ecs::Registry& registry
    ) -> bool;

    [[nodiscard]]
    static auto smallest_required_component_table_from(
        std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
            included_component_table_pointers
    ) -> ComponentTable&;

    [[nodiscard]]
    static auto matching_archetype_ids_from(ComponentTable& component_table);

    template <typename F>
    static auto visit_archetype(
        core::ecs::Registry& registry,
        std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
                    included_component_table_pointers,
        ArchetypeID archetype_id,
        F&          func
    ) -> void;

    template <size_t queried_type_index_T>
        requires(std::is_same_v<
                 util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>,
                 core::ecs::ID>)
    [[nodiscard]]
    static auto queried_type_view_from(
        core::ecs::Registry& registry,
        std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
                    included_component_table_pointers,
        ArchetypeID archetype_id
    ) -> std::span<const core::ecs::ID>;

    template <size_t queried_type_index_T>
        requires(util::meta::specialization_of_c<
                 util::meta::type_list_at_t<QueriedParameters, queried_type_index_T>,
                 core::ecs::Optional>)
    [[nodiscard]]
    static auto queried_type_view_from(
        core::ecs::Registry& registry,
        std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
                    included_component_table_pointers,
        ArchetypeID archetype_id
    ) -> OptionalView<util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>;

    template <size_t queried_type_index_T>
    [[nodiscard]]
    static auto queried_type_view_from(
        core::ecs::Registry& registry,
        std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
                    included_component_table_pointers,
        ArchetypeID archetype_id
    ) ->
        typename ComponentContainer<
            util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>::Underlying&;
};

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <::invocable_with_c<util::meta::type_list_transform_t<
    util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsQueriedParameter>,
    ToFunctionParameter>> F>
auto QueryClosure<Parameters_T...>::operator()(core::ecs::Registry& registry, F&& func)
    -> F
{
    std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>
        included_component_table_pointers{};

    if (!fill_included_component_table_pointers(
            included_component_table_pointers, registry
        ))
    {
        return std::forward<F>(func);
    };

    for (const ArchetypeID archetype_id : matching_archetype_ids_from(
             smallest_required_component_table_from(included_component_table_pointers)
         ))
    {
        visit_archetype(registry, included_component_table_pointers, archetype_id, func);
    }

    return std::forward<F>(func);
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
constexpr auto QueryClosure<Parameters_T...>::matches_archetype(const Archetype& archetype)
    -> bool
{
    return util::meta::apply<RequiredComponents>([&archetype]<typename... Ts> {
               return archetype.contains_all_of_components<Ts...>();
           })
        && util::meta::apply<ExcludedComponents>([&archetype]<typename... Ts> {
               return archetype.contains_none_of_components<Ts...>();
           });
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <core::ecs::component_c Component_T>
consteval auto QueryClosure<Parameters_T...>::include_index_of() -> size_t
{
    return util::meta::type_list_index_of_v<IncludedComponents, Component_T>;
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto QueryClosure<Parameters_T...>::fill_included_component_table_pointers(
    std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
                         included_component_table_pointers,
    core::ecs::Registry& registry
) -> bool
{
    if (util::meta::any_of<
            std::make_index_sequence<util::meta::type_list_size_v<IncludedComponents>>>(
            [&registry, &included_component_table_pointers]<size_t index_T> {
                if (const util::OptionalRef<ComponentTable> optional_component_table_ref =
                        registry.m_component_tables.find_component_table<
                            util::meta::type_list_at_t<IncludedComponents, index_T>>();
                    optional_component_table_ref.has_value())
                {
                    included_component_table_pointers[index_T] =
                        std::addressof(*optional_component_table_ref);
                    return false;
                }
                return util::meta::type_list_contains_v<
                    RequiredComponents,
                    util::meta::type_list_at_t<IncludedComponents, index_T>>;
            }
        ))
    {
        return false;
    };

    return true;
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto QueryClosure<Parameters_T...>::smallest_required_component_table_from(
    std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
        included_component_table_pointers
) -> ComponentTable&
{
    return util::meta::fold_left_first<RequiredComponents>(
        [&included_component_table_pointers]<typename Component_T> {
            return std::ref(
                *included_component_table_pointers[include_index_of<Component_T>()]
            );
        },
        [](ComponentTable& lhs, ComponentTable& rhs) {
            constexpr static auto project = &ComponentTable::size;
            if (std::invoke(project, lhs) < std::invoke(project, rhs)) {
                return std::ref(lhs);
            }
            return std::ref(rhs);
        }
    );
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto QueryClosure<Parameters_T...>::matching_archetype_ids_from(
    ComponentTable& component_table
)
{
    return component_table.underlying()
         | std::views::transform(
               &std::pair<const ArchetypeID, ErasedComponentContainer>::first
         )
         | std::views::transform(&ArchetypeID::get)
         | std::views::filter(matches_archetype)
         | std::views::transform([](const Archetype& archetype) {
               return ArchetypeID{ archetype };
           });
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <typename F>
auto QueryClosure<Parameters_T...>::visit_archetype(
    core::ecs::Registry& registry,
    std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
                      included_component_table_pointers,
    const ArchetypeID archetype_id,
    F&                func
) -> void
{
    util::meta::apply<std::make_index_sequence<util::meta::type_list_size_v<QueriedTypes>>>(
        [&, archetype_id]<size_t... indices_T> {
            std::ranges::for_each(
                std::views::zip(
                    queried_type_view_from<indices_T>(
                        registry, included_component_table_pointers, archetype_id
                    )...
                ),
                [&func](auto queried_type_tuple) { std::apply(func, queried_type_tuple); }
            );
        }
    );
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <size_t queried_type_index_T>
    requires(std::is_same_v<
             util::meta::type_list_at_t<
                 util::meta::type_list_transform_t<
                     util::meta::
                         type_list_filter_t<util::TypeList<Parameters_T...>, IsQueriedParameter>,
                     ToQueriedType>,
                 queried_type_index_T>,
             core::ecs::ID>)
auto QueryClosure<Parameters_T...>::queried_type_view_from(
    core::ecs::Registry& registry,
    std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&,
    const ArchetypeID archetype_id
) -> std::span<const core::ecs::ID>
{
    return registry.m_lookup_tables.get_iterator(archetype_id)->second.ids();
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <size_t queried_type_index_T>
    requires(util::meta::specialization_of_c<
             util::meta::type_list_at_t<
                 util::meta::
                     type_list_filter_t<util::TypeList<Parameters_T...>, IsQueriedParameter>,
                 queried_type_index_T>,
             core::ecs::Optional>)
auto QueryClosure<Parameters_T...>::queried_type_view_from(
    core::ecs::Registry&,
    std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
                      included_component_table_pointers,
    const ArchetypeID archetype_id
) -> OptionalView<util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>
{
    ComponentTable* const component_table_ptr{
        included_component_table_pointers[include_index_of<
            util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>()]
    };
    if (component_table_ptr == nullptr) {
        return OptionalView<
            util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>{};
    }

    util::OptionalRef optional_component_container{
        component_table_ptr->find_component_container<
            util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>(archetype_id)
    };
    if (!optional_component_container.has_value()) {
        return OptionalView<
            util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>{};
    }

    return OptionalView{ std::span{ optional_component_container->underlying() } };
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <size_t queried_type_index_T>
auto QueryClosure<Parameters_T...>::queried_type_view_from(
    core::ecs::Registry&,
    std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>&
                      included_component_table_pointers,
    const ArchetypeID archetype_id
) ->
    typename ComponentContainer<
        util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>::Underlying&
{
    return included_component_table_pointers
        [include_index_of<util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>()]
            ->template get_component_container<
                util::meta::type_list_at_t<QueriedTypes, queried_type_index_T>>(
                archetype_id
            )
            .underlying();
}
