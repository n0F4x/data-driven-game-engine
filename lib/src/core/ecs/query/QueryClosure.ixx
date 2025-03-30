module;

#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <ranges>
#include <tuple>
#include <type_traits>

export module core.ecs:query.QueryClosure;

import utility.containers.OptionalRef;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_index_of;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.type_list.type_list_size;
import utility.meta.type_traits.underlying;
import utility.TypeList;

import :ComponentContainer;
import :ComponentTable;
import :ID;
import :query.component_query_parameter_c;
import :query.OptionalView;
import :query.queryable_component_c;
import :query.QueryClosure.fwd;
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
struct ToFunctionParameter<T>
    : std::type_identity<util::OptionalRef<util::meta::underlying_t<T>>> {};

template <typename, typename>
struct invocable_with_impl;

template <typename F, template <typename...> typename TypeList_T, typename... ForEachParameters_T>
struct invocable_with_impl<F, TypeList_T<ForEachParameters_T...>>
    : std::bool_constant<std::invocable<F, ForEachParameters_T...>> {};

template <typename F, typename ForEachParameterTypeList_T>
concept invocable_with_c = invocable_with_impl<F, ForEachParameterTypeList_T>::value;

namespace core::ecs {

template <query_parameter_c... Parameters_T>
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

    using MustIncludeComponents = util::meta::type_list_transform_t<
        util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsComponentOrWith>,
        ToComponent>;

    static_assert(
        util::meta::type_list_size_v<MustIncludeComponents> != 0,
        "query must have required components"
    );

    using ExcludedComponents = util::meta::type_list_transform_t<
        util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsWithout>,
        ToComponent>;

public:
    template <::invocable_with_c<FunctionParameters> F>
    static auto operator()(Registry& registry, F&& func) -> F
        requires(util::meta::type_list_size_v<FunctionParameters> != 0);

    [[nodiscard]]
    constexpr static auto matches_archetype(const Archetype& archetype) -> bool;

private:
    template <component_c Component_T>
    [[nodiscard]]
    consteval static auto include_index_of() -> size_t;
};

}   // namespace core::ecs

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <::invocable_with_c<util::meta::type_list_transform_t<
    util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsQueriedParameter>,
    ToFunctionParameter>> F>
auto core::ecs::QueryClosure<Parameters_T...>::operator()(
    Registry& registry,
    F&&       func
) -> F
    requires(util::meta::type_list_size_v<FunctionParameters> != 0)
{
    std::array<ComponentTable*, util::meta::type_list_size_v<IncludedComponents>>
        included_component_table_pointers{};

    IncludedComponents::enumerate(
        [&registry,
         &included_component_table_pointers]<size_t index_T, typename Component_T> {
            if (const auto iterator =
                    registry.m_component_tables.find(component_id_of<Component_T>());
                iterator != registry.m_component_tables.cend())
            {
                included_component_table_pointers[index_T] =
                    std::addressof(iterator->second);
            }
        }
    );

    if (MustIncludeComponents::apply(
            [&included_component_table_pointers]<typename... Components_T> {
                return (
                    (included_component_table_pointers[include_index_of<Components_T>()]
                     == nullptr)
                    || ...
                );
            }
        ))
    {
        return std::forward<F>(func);
    }

    ComponentTable& smallest_must_include_component_table =
        MustIncludeComponents::fold_left_first(
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

    std::ranges::for_each(
        smallest_must_include_component_table
            | std::views::transform(
                &std::pair<const ArchetypeID, ErasedComponentContainer>::first
            )
            | std::views::filter(matches_archetype),
        [&registry,
         &func,
         &included_component_table_pointers](const ArchetypeID archetype_id) {
            [&registry,
             &func,
             &included_component_table_pointers,
             archetype_id]<size_t... indices_T>(std::index_sequence<indices_T...>) {
                std::ranges::for_each(
                    std::views::zip(
                        [&registry,
                         &included_component_table_pointers,
                         archetype_id] -> decltype(auto) {
                            std::ignore = registry;
                            std::ignore = included_component_table_pointers;

                            if constexpr (std::is_same_v<
                                              util::meta::
                                                  type_list_at_t<QueriedTypes, indices_T>,
                                              core::ecs::ID>)
                            {
                                return registry.m_archetypes.find(archetype_id)
                                    ->second.ids();
                            }
                            else if constexpr (util::meta::specialization_of_c<
                                                   util::meta::type_list_at_t<
                                                       QueriedParameters,
                                                       indices_T>,
                                                   core::ecs::Optional>)
                            {
                                ComponentTable* const component_table_ptr{
                                    included_component_table_pointers[include_index_of<
                                        util::meta::type_list_at_t<QueriedTypes, indices_T>>(
                                    )]
                                };
                                if (component_table_ptr == nullptr) {
                                    return OptionalView<
                                        util::meta::
                                            type_list_at_t<QueriedTypes, indices_T>>{};
                                }

                                const auto component_table_iterator =
                                    component_table_ptr->find(archetype_id);
                                if (component_table_iterator
                                    == component_table_ptr->cend()) {
                                    return OptionalView<
                                        util::meta::
                                            type_list_at_t<QueriedTypes, indices_T>>{};
                                }

                                return OptionalView{ std::span{
                                    component_table_iterator->second.get<ComponentContainer<
                                        util::meta::type_list_at_t<QueriedTypes, indices_T>>>(
                                    ) } };
                            }
                            else {
                                return included_component_table_pointers
                                    [include_index_of<
                                         util::meta::
                                             type_list_at_t<QueriedTypes, indices_T>>()]
                                        ->find(archetype_id)
                                        ->second.template get<ComponentContainer<
                                            util::meta::
                                                type_list_at_t<QueriedTypes, indices_T>>>(
                                        );
                            }
                        }()...
                    ),
                    [&func](auto components_tuple) { std::apply(func, components_tuple); }
                );
            }(std::make_index_sequence<util::meta::type_list_size_v<QueriedTypes>>{});
        }
    );

    return std::forward<F>(func);
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
constexpr auto core::ecs::QueryClosure<Parameters_T...>::matches_archetype(
    const Archetype& archetype
) -> bool
{
    return MustIncludeComponents::apply([&archetype]<typename... Ts> {
               return archetype.contains_components<Ts...>();
           })
        && ExcludedComponents::apply([&archetype]<typename... Ts> {
               return archetype.contains_none_of_components<Ts...>();
           });
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <core::ecs::component_c Component_T>
consteval auto core::ecs::QueryClosure<Parameters_T...>::include_index_of()
    -> size_t
{
    return util::meta::type_list_index_of_v<IncludedComponents, Component_T>;
}
