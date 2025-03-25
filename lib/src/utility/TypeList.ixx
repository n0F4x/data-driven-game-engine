module;

#include <functional>
#include <utility>

export module utility.TypeList;

import utility.meta.type_traits.integer_sequence.integer_sequence_offset;

namespace util {

export template <typename... Ts>
struct TypeList {
    template <typename Transform_T>
    constexpr static auto for_each(Transform_T transform) -> Transform_T;

    template <typename IndexedTransform_T>
    constexpr static auto enumerate(IndexedTransform_T transform) -> IndexedTransform_T;

    template <typename Transform_T, typename Operation_T>
    constexpr static auto fold_left_first(Transform_T transform, Operation_T operation)
        requires(sizeof...(Ts) > 0);

    template <typename IndexedTransform_T, typename Operation_T>
    constexpr static auto
        fold_left_first_enumerate(IndexedTransform_T transform, Operation_T operation)
        requires(sizeof...(Ts) > 0);

    template <typename F>
    [[nodiscard]]
    constexpr static auto apply(F func);
};

}   // namespace util

template <typename... Ts>
template <typename Transform_T>
constexpr auto util::TypeList<Ts...>::for_each(Transform_T transform) -> Transform_T
{
    [&transform]<size_t... indices_T>(std::index_sequence<indices_T...>) {
        (transform.template operator()<Ts...[indices_T]>(), ...);
    }(std::make_index_sequence<sizeof...(Ts)>{});

    return transform;
}

template <typename... Ts>
template <typename IndexedTransform_T>
constexpr auto util::TypeList<Ts...>::enumerate(IndexedTransform_T transform)
    -> IndexedTransform_T
{
    [&transform]<size_t... indices_T>(std::index_sequence<indices_T...>) {
        (transform.template operator()<indices_T, Ts...[indices_T]>(), ...);
    }(std::make_index_sequence<sizeof...(Ts)>{});

    return transform;
}

template <typename... Ts>
template <typename Transform_T, typename Operation_T>
constexpr auto
    util::TypeList<Ts...>::fold_left_first(Transform_T transform, Operation_T operation)
    requires(sizeof...(Ts) > 0)
{
    return [&transform,
            &operation]<size_t current_index_T, typename Self_T, typename Accumulated_T>(
               this Self_T self, Accumulated_T&& accumulated
           ) {
        if constexpr (current_index_T == sizeof...(Ts)) {
            return std::forward<Accumulated_T>(accumulated);
        }
        else {
            return self.template operator()<current_index_T + 1>(std::invoke(
                operation,
                std::forward<Accumulated_T>(accumulated),
                transform.template operator()<Ts...[current_index_T]>()
            ));
        }
    }.template operator()<1>(transform.template operator()<Ts...[0]>());
}

template <typename... Ts>
template <typename IndexedTransform_T, typename Operation_T>
constexpr auto util::TypeList<Ts...>::fold_left_first_enumerate(
    IndexedTransform_T transform,
    Operation_T        operation
)
    requires(sizeof...(Ts) > 0)
{
    return [&transform,
            &operation]<size_t current_index_T, typename Self_T, typename Accumulated_T>(
               this Self_T self, Accumulated_T&& accumulated
           ) {
        if constexpr (current_index_T == sizeof...(Ts)) {
            return std::forward<Accumulated_T>(accumulated);
        }
        else {
            return self.template operator()<current_index_T + 1>(std::invoke(
                operation,
                std::forward<Accumulated_T>(accumulated),
                transform.template operator()<current_index_T, Ts...[current_index_T]>()
            ));
        }
    }.template operator()<1>(transform.template operator()<0, Ts...[0]>());
}

template <typename... Ts>
template <typename F>
constexpr auto util::TypeList<Ts...>::apply(F func)
{
    return func.template operator()<Ts...>();
}
