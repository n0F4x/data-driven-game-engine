module;

#include <functional>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <entt/entt.hpp>

export module ecs:Query;

import :Registry;

import utility.meta.concepts.type_list.type_list_c;
import utility.meta.concepts.specialization_of;

import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_chunk_sub;
import utility.meta.type_traits.type_list.type_list_concat;
import utility.meta.type_traits.type_list.type_list_disjoin;
import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_join;
import utility.meta.type_traits.type_list.type_list_size;
import utility.meta.type_traits.type_list.type_list_to;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.type_list.type_list_unique;
import utility.meta.type_traits.add_wrapper;
import utility.meta.type_traits.is_specialization_of;
import utility.meta.type_traits.remove_wrapper_if;
import utility.meta.type_traits.underlying;

import utility.containers.OptionalRef;
import utility.tuple;

namespace ecs {

template <typename T>
struct is_queryable_component : std::negation<std::is_empty<T>> {};

export template <typename...>
struct Without {};

export template <typename...>
struct With {};

export template <typename...>
struct Optional {};

export struct Entity {};

template <typename T>
struct is_specialization_of_Without : util::meta::is_specialization_of<T, Without> {};

template <typename T>
struct is_specialization_of_With : util::meta::is_specialization_of<T, With> {};

template <typename T>
struct is_specialization_of_Optional : util::meta::is_specialization_of<T, Optional> {};

template <typename T>
struct is_Entity : std::is_same<std::remove_const_t<T>, Entity> {};

template <typename T>
struct is_component : std::conjunction<
                          std::negation<std::disjunction<
                              is_specialization_of_Without<T>,
                              is_specialization_of_With<T>,
                              is_specialization_of_Optional<T>,
                              is_Entity<T>>>,
                          is_queryable_component<T>> {};

template <typename>
struct is_queryable_Optional : std::false_type {};

template <typename... Ts>
    requires((!std::is_empty_v<Ts>) && ...)
struct is_queryable_Optional<Optional<Ts...>> : std::true_type {};

export template <typename T>
concept queryable_c = std::disjunction_v<
    is_queryable_component<T>,
    is_specialization_of_Without<T>,
    is_specialization_of_With<T>,
    is_queryable_Optional<T>,
    is_Entity<T>>;

template <typename T>
struct get_component : std::type_identity<T> {};

template <typename T>
struct get_component<Without<T>> {
    using type = T;
};

template <typename T>
struct get_component<With<T>> {
    using type = T;
};

template <typename T>
struct get_component<Optional<T>> {
    using type = T;
};

template <typename T>
using chunk_specialized_filters_t = util::meta::type_list_chunk_sub_t<
    util::meta::type_list_chunk_sub_t<util::meta::type_list_chunk_sub_t<T, Without>, With>,
    Optional>;

template <typename... Filters_T>
concept no_duplicate_filter =
    util::meta::type_list_size_v<util::meta::type_list_transform_t<
        util::meta::type_list_transform_t<
            chunk_specialized_filters_t<std::tuple<Filters_T...>>,
            get_component>,
        std::remove_const>>
    == util::meta::
        type_list_size_v<util::meta::type_list_unique_t<util::meta::type_list_transform_t<
            util::meta::type_list_transform_t<
                chunk_specialized_filters_t<std::tuple<Filters_T...>>,
                get_component>,
            std::remove_const>>>;

// TODO: finish Entity

export template <queryable_c... Filters_T>
    requires(no_duplicate_filter<Filters_T...>)
class Query {
    using Components =
        util::meta::type_list_filter_t<std::tuple<Filters_T...>, is_component>;
    using WithoutComponents = util::meta::type_list_join_t<
        util::meta::
            type_list_filter_t<std::tuple<Filters_T...>, is_specialization_of_Without>>;
    using WithComponents = util::meta::type_list_join_t<
        util::meta::type_list_filter_t<std::tuple<Filters_T...>, is_specialization_of_With>>;
    using OptionalComponents = util::meta::type_list_join_t<
        util::meta::
            type_list_filter_t<std::tuple<Filters_T...>, is_specialization_of_Optional>>;

    using IncludedComponents = util::meta::type_list_concat_t<Components, WithComponents>;
    using ExcludedComponents = WithoutComponents;

    template <typename T>
    struct add_optional {
        using type = std::optional<T>;
    };

    template <typename T>
    struct is_component_or_Optional
        : std::disjunction<is_component<T>, is_specialization_of_Optional<T>> {};

    using ExtendedComponents = util::meta::type_list_chunk_sub_t<
        util::meta::type_list_filter_t<std::tuple<Filters_T...>, is_component_or_Optional>,
        Optional>;

    using View = entt::view<
        util::meta::type_list_to_t<IncludedComponents, entt::get_t>,
        util::meta::type_list_to_t<ExcludedComponents, entt::exclude_t>>;

    template <typename T>
    struct storage_for_component {
        // TODO: don't remove_const once entt commit 86c3c45 comes through
        using type = std::remove_const_t<entt::storage_for_t<T>>;
    };

    template <typename T>
    using storage_for_component_t = typename storage_for_component<T>::type;

    template <typename T>
    struct add_reference_wrapper : util::meta::add_wrapper<T, std::reference_wrapper> {};

    using OptionalStoragesTuple = util::meta::type_list_to_t<
        util::meta::type_list_transform_t<
            util::meta::type_list_unique_t<
                util::meta::type_list_transform_t<OptionalComponents, storage_for_component>>,
            add_reference_wrapper>,
        std::tuple>;

public:
    class Iterator;

    explicit Query(entt::registry& registry);

    auto begin() const -> Iterator;
    auto end() const -> Iterator;

    template <typename F>
    auto each(F&& func) const -> void;

private:
    friend Iterator;

    View                  m_view;
    OptionalStoragesTuple m_optional_storages;
};

template <queryable_c... Filters_T>
    requires(no_duplicate_filter<Filters_T...>)
class Query<Filters_T...>::Iterator {
    template <typename T>
    struct convert_to_proper_reference {
        using type = T&;
    };

    template <typename T>
        requires(is_specialization_of_Optional<T>::value)
    struct convert_to_proper_reference<T> {
        using type = util::OptionalRef<util::meta::underlying_t<T>>;
    };

    using Value = util::meta::type_list_to_t<
        util::meta::type_list_transform_t<ExtendedComponents, convert_to_proper_reference>,
        std::tuple>;

public:
    [[nodiscard]]
    auto operator==(Iterator other) const -> bool;
    auto operator++() -> Iterator&;
    auto operator++(int) -> Iterator;
    [[nodiscard]]
    auto operator*() const -> Value;

private:
    friend Query;

    typename View::iterator m_iterator;
    Query                   m_query;

    Iterator(typename View::iterator iterator, Query query);
};

}   // namespace ecs

template <ecs::queryable_c... Filters_T>
    requires(ecs::no_duplicate_filter<Filters_T...>)
auto ecs::Query<Filters_T...>::Iterator::operator==(Iterator other) const -> bool
{
    return m_iterator == other.m_iterator;
}

template <ecs::queryable_c... Filters_T>
    requires(ecs::no_duplicate_filter<Filters_T...>)
auto ecs::Query<Filters_T...>::Iterator::operator++() -> Iterator&
{
    ++m_iterator;
    return *this;
}

template <ecs::queryable_c... Filters_T>
    requires(ecs::no_duplicate_filter<Filters_T...>)
auto ecs::Query<Filters_T...>::Iterator::operator++(int) -> Iterator
{
    Iterator tmp{ *this };
    ++(*this);
    return tmp;
}

template <ecs::queryable_c... Filters_T>
    requires(ecs::no_duplicate_filter<Filters_T...>)
auto ecs::Query<Filters_T...>::Iterator::operator*() const -> Value
{
    return util::generate_tuple_from<ExtendedComponents>(
        [entity = *m_iterator, this]<typename Component>() -> decltype(auto) {
            if constexpr (is_component<Component>::value) {
                return m_query.m_view.template get<Component>(entity);
            }
            else if constexpr (is_specialization_of_Optional<Component>::value) {
                using RealComp   = util::meta::underlying_t<Component>;
                using ReturnType = util::OptionalRef<RealComp>;

                if (auto& storage{
                        std::get<std::reference_wrapper<storage_for_component_t<RealComp>>>(
                            m_query.m_optional_storages
                        )
                            .get() };
                    storage.contains(entity))
                {
                    return ReturnType{ storage.get(entity) };
                }

                return ReturnType{};
            }
            else {
                static_assert(false);
            }
        }
    );
}

template <ecs::queryable_c... Filters_T>
    requires(ecs::no_duplicate_filter<Filters_T...>)
ecs::Query<Filters_T...>::Iterator::Iterator(typename View::iterator iterator, Query query)
    : m_iterator{ iterator },
      m_query{ query }
{}

template <typename TypeList_T>
struct collector;

template <template <typename...> typename TypeList_T, typename... Ts>
struct collector<TypeList_T<Ts...>> {
    template <typename TemplatedLambda_T>
    [[nodiscard]]
    static auto operator()(TemplatedLambda_T&& lambda)
    {
        return std::forward<TemplatedLambda_T>(lambda).template operator()<Ts...>();
    }
};

template <ecs::queryable_c... Filters_T>
    requires(ecs::no_duplicate_filter<Filters_T...>)
ecs::Query<Filters_T...>::Query(entt::registry& registry)
    : m_view{ collector<IncludedComponents>::operator()([&registry]<typename... Ts>() {
          using ExcludeTag =
              util::meta::type_list_to_t<ExcludedComponents, entt::exclude_t>;
          return registry.view<Ts...>(ExcludeTag{});
      }) },
      // TODO: don't remove_const once entt commit 86c3c45 comes through
      m_optional_storages{ util::generate_tuple_from<util::meta::type_list_unique_t<
          util::meta::type_list_transform_t<OptionalComponents, std::remove_const>>>(
          [&registry]<typename Component> {
              return std::ref(registry.storage<Component>());
          }
      ) }
{}

template <ecs::queryable_c... Filters_T>
    requires(ecs::no_duplicate_filter<Filters_T...>)
auto ecs::Query<Filters_T...>::begin() const -> Iterator
{
    return Iterator{ m_view.begin(), *this };
}

template <ecs::queryable_c... Filters_T>
    requires(ecs::no_duplicate_filter<Filters_T...>)
auto ecs::Query<Filters_T...>::end() const -> Iterator
{
    return Iterator{ m_view.end(), *this };
}

template <ecs::queryable_c... Filters_T>
    requires(ecs::no_duplicate_filter<Filters_T...>)
template <typename F>
auto ecs::Query<Filters_T...>::each(F&& func) const -> void
{
    for (auto&& tuple : *this) {
        std::apply(func, std::forward<decltype(tuple)>(tuple));
    }
}
