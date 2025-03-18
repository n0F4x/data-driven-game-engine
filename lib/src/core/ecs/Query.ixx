module;

#include <algorithm>
#include <concepts>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <vector>

export module core.ecs:Query;

import utility.meta.concepts.decayed;
import utility.meta.type_traits.all_different;
import utility.meta.type_traits.integer_sequence.integer_sequence_offset;
import utility.ValueSequence;

import :Component;
import :fwd;
import :Registry;
import :specialization_of_registry_c;

namespace core::ecs {

export template <typename... Components_T>
    requires(queryable_component_c<std::remove_const_t<Components_T>> && ...)
         && (::util::meta::all_different_v<std::remove_const_t<Components_T>...>)
class Query {
public:
    template <specialization_of_registry_c Registry_T>
        requires(::util::meta::decayed_c<Registry_T>)
    explicit Query(Registry_T& registry);

    template <std::invocable<Components_T&...> F>
    auto each(F func) -> void
        requires(sizeof...(Components_T) > 0);

private:
    std::array<std::reference_wrapper<::ComponentTable>, sizeof...(Components_T)>
        m_component_table_refs;

    template <specialization_of_registry_c Registry_T>
        requires(::util::meta::decayed_c<Registry_T>)
    [[nodiscard]]
    auto create_component_table_refs(Registry_T& registry)
        -> std::array<std::reference_wrapper<::ComponentTable>, sizeof...(Components_T)>;
};

}   // namespace core::ecs

template <typename... Components_T>
    requires(core::ecs::queryable_component_c<std::remove_const_t<Components_T>> && ...)
         && (::util::meta::all_different_v<std::remove_const_t<Components_T>...>)
            template <core::ecs::specialization_of_registry_c Registry_T>
                requires(::util::meta::decayed_c<Registry_T>)
core::ecs::Query<Components_T...>::Query(Registry_T& registry)
    : m_component_table_refs{ create_component_table_refs(registry) }
{}

template <typename... Components_T>
    requires(core::ecs::queryable_component_c<std::remove_const_t<Components_T>> && ...)
         && (::util::meta::all_different_v<std::remove_const_t<Components_T>...>)
            template <std::invocable<Components_T&...> F>
            auto core::ecs::Query<Components_T...>::each(F func) -> void
                requires(sizeof...(Components_T) > 0)
{
    [this, func]<
        size_t... indices_T,
        size_t... extra_indices_T>(std::index_sequence<indices_T...>, std::index_sequence<extra_indices_T...>) {
        std::array<ComponentTable::iterator, sizeof...(Components_T) - 1> extra_iterators{
            m_component_table_refs[extra_indices_T].get().begin()...
        };

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

            std::array<
                std::reference_wrapper<ErasedComponentContainer>,
                sizeof...(Components_T)>
                erased_component_containers{
                    main_erased_component_container,
                    extra_iterators[extra_indices_T - 1]->second...
                };

            for (auto&& components_tuple : std::views::zip(
                     erased_component_containers[indices_T]
                         .get()
                         .template get<ComponentContainer<Components_T>>()...
                 ))
            {
                std::invoke(func, std::get<indices_T>(components_tuple)...);
            }
        }
    }(std::make_index_sequence<sizeof...(Components_T)>{},
      util::meta::integer_sequence_offset_t<
          std::make_index_sequence<sizeof...(Components_T) - 1>,
          1>{});
}

template <typename... Components_T>
    requires(core::ecs::queryable_component_c<std::remove_const_t<Components_T>> && ...)
         && (::util::meta::all_different_v<std::remove_const_t<Components_T>...>)
            template <core::ecs::specialization_of_registry_c Registry_T>
                requires(::util::meta::decayed_c<Registry_T>)
auto core::ecs::Query<Components_T...>::create_component_table_refs(Registry_T& registry)
    -> std::array<std::reference_wrapper<::ComponentTable>, sizeof...(Components_T)>
{
    return std::array<std::reference_wrapper<::ComponentTable>, sizeof...(Components_T)>{
        registry.m_component_tables[::component_id_v<Components_T>]...
    };
}
