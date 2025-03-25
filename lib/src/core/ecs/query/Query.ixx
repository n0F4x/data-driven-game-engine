module;

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <functional>
#include <ranges>
#include <tuple>
#include <type_traits>

#include "utility/lifetime_bound.hpp"

export module core.ecs:query.Query;

import utility.containers.OptionalRef;
import utility.meta.concepts.decayed;
import utility.meta.type_traits.integer_sequence.integer_sequence_offset;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.type_list.type_list_push_front;
import utility.meta.type_traits.type_list.type_list_size;
import utility.meta.type_traits.underlying;
import utility.TypeList;
import utility.ValueSequence;

import :ArchetypeContainer;
import :ComponentContainer;
import :ComponentTable;
import :ID;
import :query.queryable_component_c;
import :Registry;

template <typename T>
struct IsOptionalOrNotTagged {
    constexpr static bool value = core::ecs::component_c<std::remove_const_t<T>>
                               || util::meta::specialization_of_c<T, core::ecs::Optional>;
};

template <typename T>
struct IsWithOrNotTagged {
    constexpr static bool value = core::ecs::component_c<std::remove_const_t<T>>
                               || util::meta::specialization_of_c<T, core::ecs::With>;
};

template <typename T>
struct IsWithout {
    constexpr static bool value = util::meta::specialization_of_c<T, core::ecs::Without>;
};

template <typename T>
struct ToFunctionParameter : std::type_identity<std::add_lvalue_reference_t<T>> {};

template <util::meta::specialization_of_c<core::ecs::Optional> T>
struct ToFunctionParameter<T>
    : std::type_identity<util::OptionalRef<util::meta::underlying_t<T>>> {};

template <typename... QueryParameters_T>
using for_each_parameters_t = util::meta::type_list_transform_t<
    util::meta::
        type_list_filter_t<util::TypeList<QueryParameters_T...>, IsOptionalOrNotTagged>,
    ToFunctionParameter>;

template <typename, typename>
struct invocable_with_impl;

template <typename F, template <typename...> typename TypeList_T, typename... ForEachParameters_T>
struct invocable_with_impl<F, TypeList_T<ForEachParameters_T...>>
    : std::bool_constant<std::invocable<F, ForEachParameters_T...>> {};

template <typename F, typename ForEachParameterTypeList_T>
concept invocable_with_c = invocable_with_impl<F, ForEachParameterTypeList_T>::value;

template <typename F, typename ForEachParameterTypeList_T>
concept enumerate_func_c = invocable_with_c<
    F,
    util::meta::type_list_push_front_t<ForEachParameterTypeList_T, core::ecs::ID>>;

template <typename T>
struct ToComponent : std::type_identity<std::remove_const_t<T>> {};

template <core::ecs::query_parameter_tag_c T>
struct ToComponent<T>
    : std::type_identity<std::remove_const_t<util::meta::underlying_t<T>>> {};

namespace core::ecs {

export template <query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
class Query {
    using ForEachParameters = for_each_parameters_t<Parameters_T...>;

    using QueriedComponents = util::meta::type_list_transform_t<
        util::meta::
            type_list_filter_t<util::TypeList<Parameters_T...>, IsOptionalOrNotTagged>,
        ToComponent>;

    using MustIncludeComponents = util::meta::type_list_transform_t<
        util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsWithOrNotTagged>,
        ToComponent>;

    using ExcludedComponents = util::meta::type_list_transform_t<
        util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsWithout>,
        ToComponent>;

public:
    explicit Query(Registry& [[lifetime_bound]] registry);

    template <::invocable_with_c<ForEachParameters> F>
    auto each(F&& func) -> F
        requires(util::meta::type_list_size_v<ForEachParameters> != 0);

    template <::enumerate_func_c<ForEachParameters> F>
    auto each(F&& func) -> F;

private:
    std::array<
        std::reference_wrapper<::ComponentTable>,
        util::meta::type_list_size_v<QueriedComponents>>
                                                 m_component_table_refs;
    std::reference_wrapper<::ArchetypeContainer> m_archetypes;

    [[nodiscard]]
    auto create_component_table_refs(Registry& registry) -> std::array<
        std::reference_wrapper<::ComponentTable>,
        util::meta::type_list_size_v<QueriedComponents>>;

    [[nodiscard]]
    auto matches_archetype(const Archetype& archetype) const -> bool;
};

}   // namespace core::ecs

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
core::ecs::Query<Parameters_T...>::Query(Registry& registry)
    : m_component_table_refs{ create_component_table_refs(registry) },
      m_archetypes{ registry.m_archetypes }
{}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <::invocable_with_c<util::meta::type_list_transform_t<
    util::meta::type_list_filter_t<util::TypeList<Parameters_T...>, IsOptionalOrNotTagged>,
    ToFunctionParameter>> F>
auto core::ecs::Query<Parameters_T...>::each(F&& func) -> F
    requires(
        util::meta::type_list_size_v<util::meta::type_list_transform_t<
            util::meta::
                type_list_filter_t<util::TypeList<Parameters_T...>, IsOptionalOrNotTagged>,
            ToFunctionParameter>>
        != 0
    )
{
    // TODO: factor out to custom set_intersection algorithm

    [this, func]<
        size_t... indices_T,
        size_t... extra_indices_T>(std::index_sequence<indices_T...>, std::index_sequence<extra_indices_T...>) {
        std::array<
            ComponentTable::iterator,
            util::meta::type_list_size_v<QueriedComponents> - 1>
            extra_iterators{ m_component_table_refs[extra_indices_T].get().begin()... };

        for (auto& [main_archetype_id, main_erased_component_container] :
             m_component_table_refs[0].get())
        {
            bool skip_archetype{};
            for (auto&& [component_table_index, extra_iterator] :
                 std::views::zip(std::views::iota(1uz), extra_iterators))
            {
                while (extra_iterator->first < main_archetype_id
                       && extra_iterator
                              != m_component_table_refs[component_table_index].get().cend(
                              ))
                {
                    ++extra_iterator;
                }

                if (extra_iterator
                    == m_component_table_refs[component_table_index].get().cend())
                {
                    return;
                }

                if (extra_iterator->first != main_archetype_id) {
                    skip_archetype = true;
                    break;
                }
            }

            if (skip_archetype) {
                continue;
            }

            assert(
                (extra_iterators[extra_indices_T - 1]->second.size()
                 == main_erased_component_container.size())
                && ...
            );

            std::array<
                std::reference_wrapper<ErasedComponentContainer>,
                util::meta::type_list_size_v<QueriedComponents>>
                erased_component_containers{
                    main_erased_component_container,
                    extra_iterators[extra_indices_T - 1]->second...
                };

            std::ranges::for_each(
                std::views::zip(
                    erased_component_containers[indices_T]
                        .get()
                        .template get<ComponentContainer<
                            util::meta::type_list_at_t<QueriedComponents, indices_T>>>()...
                ),
                [&func]<typename ComponentsTuple_T>(ComponentsTuple_T&& components_tuple) {
                    std::apply(func, components_tuple);
                }
            );
        }
    }(std::make_index_sequence<util::meta::type_list_size_v<QueriedComponents>>{},
      util::meta::integer_sequence_offset_t<
          std::make_index_sequence<util::meta::type_list_size_v<QueriedComponents> - 1>,
          1>{});

    return std::forward<F>(func);
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
template <::enumerate_func_c<for_each_parameters_t<Parameters_T...>> F>
auto core::ecs::Query<Parameters_T...>::each(F&& func) -> F
{
    if constexpr (util::meta::type_list_size_v<QueriedComponents> == 0) {
        std::ranges::for_each(
            m_archetypes.get() | std::views::values
                | std::views::transform(&Archetype::ids) | std::views::join,
            func
        );
    }
    else {
        // TODO: factor out to custom set_intersection algorithm

        [this, func]<
            size_t... indices_T,
            size_t... extra_indices_T>(std::index_sequence<indices_T...>, std::index_sequence<extra_indices_T...>) {
            std::array<
                ComponentTable::iterator,
                util::meta::type_list_size_v<QueriedComponents> - 1>
                extra_iterators{ m_component_table_refs[extra_indices_T].get().begin(
                )... };

            for (auto& [main_archetype_id, main_erased_component_container] :
                 m_component_table_refs[0].get())
            {
                bool skip_archetype{};
                for (auto&& [component_table_index, extra_iterator] :
                     std::views::zip(std::views::iota(1uz), extra_iterators))
                {
                    while (
                        extra_iterator->first < main_archetype_id
                        && extra_iterator
                               != m_component_table_refs[component_table_index].get().cend(
                               )
                    )
                    {
                        ++extra_iterator;
                    }

                    if (extra_iterator
                        == m_component_table_refs[component_table_index].get().cend())
                    {
                        return;
                    }

                    if (extra_iterator->first != main_archetype_id) {
                        skip_archetype = true;
                        break;
                    }
                }

                if (skip_archetype) {
                    continue;
                }

                std::array<
                    std::reference_wrapper<ErasedComponentContainer>,
                    util::meta::type_list_size_v<QueriedComponents>>
                    erased_component_containers{
                        main_erased_component_container,
                        extra_iterators[extra_indices_T - 1]->second...
                    };

                const Archetype& archetype{
                    m_archetypes.get().find(main_archetype_id)->second
                };

                assert(
                    (erased_component_containers[indices_T].get().size()
                     == archetype.ids().size())
                    && ...
                );

                std::ranges::for_each(
                    std::views::zip(
                        archetype.ids(),
                        erased_component_containers[indices_T]
                            .get()
                            .template get<ComponentContainer<
                                util::meta::type_list_at_t<QueriedComponents, indices_T>>>(
                            )...
                    ),
                    [&func]<typename Params_T>(Params_T&& tuple) {
                        std::apply(func, std::forward<Params_T>(tuple));
                    }
                );
            }
        }(std::make_index_sequence<util::meta::type_list_size_v<QueriedComponents>>{},
          util::meta::integer_sequence_offset_t<
              std::make_index_sequence<util::meta::type_list_size_v<QueriedComponents> - 1>,
              1>{});
    }

    return std::forward<F>(func);
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto core::ecs::Query<Parameters_T...>::create_component_table_refs(Registry& registry)
    -> std::array<
        std::reference_wrapper<::ComponentTable>,
        util::meta::type_list_size_v<QueriedComponents>>
{
    return [&registry]<size_t... indices_T>(std::index_sequence<indices_T...>) {
        return std::array<
            std::reference_wrapper<::ComponentTable>,
            util::meta::type_list_size_v<QueriedComponents>>{
            registry.m_component_tables[component_id<
                util::meta::type_list_at_t<QueriedComponents, indices_T>>]...
        };
    }(std::make_index_sequence<util::meta::type_list_size_v<QueriedComponents>>{});
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires ::query_parameter_components_are_all_different_c<Parameters_T...>
auto core::ecs::Query<Parameters_T...>::matches_archetype(const Archetype& archetype) const
    -> bool
{
    return MustIncludeComponents::apply([&archetype]<typename... Ts> {
               return archetype.contains_components<Ts...>();
           })
        && ExcludedComponents::apply([&archetype]<typename... Ts> {
               return archetype.contains_none_of_components<Ts...>();
           });
}
