module;

#include <concepts>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

export module utility.containers.StackedTuple;

import utility.meta.algorithms.for_each;
import utility.meta.concepts.integer_sequence.index_sequence;
import utility.meta.concepts.storable;
import utility.meta.reflection.name_of;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_index_of;
import utility.TypeList;

template <size_t, typename T>
struct Leaf {
    T value;
};

template <typename Factory_T, typename T>
concept factory_for_c = std::constructible_from<T, util::meta::result_of_t<Factory_T>>;

template <util::meta::index_sequence_c, typename... Ts>
struct Impl;

template <template <typename T_, T_...> typename IntegerSequence_T>
struct Impl<IntegerSequence_T<size_t>> {
    constexpr explicit Impl(std::tuple<>);
};

template <template <typename T_, T_...> typename IntegerSequence_T, size_t I, typename T>
struct Impl<IntegerSequence_T<size_t, I>, T> : Leaf<I, T> {
    template <typename... Stacked_T, typename Factory_T>
    constexpr Impl(std::tuple<Stacked_T&...> stack, Factory_T&& factory);
};

template <
    template <typename T_, T_...> typename IntegerSequence_T,
    size_t I,
    size_t... Is,
    typename T,
    typename... Ts>
struct Impl<IntegerSequence_T<size_t, I, Is...>, T, Ts...>
    : Leaf<I, T>, Impl<IntegerSequence_T<size_t, Is...>, Ts...> {
    template <typename... Stacked_T, typename Factory_T, typename... Factories_T>
    constexpr Impl(
        std::tuple<Stacked_T&...> stack,
        Factory_T&&               factory,
        Factories_T&&... factories
    );
};

namespace util {

export template <::util::meta::storable_c... Ts>
class StackedTuple {
public:
    template <typename... Factories_T>
        requires(factory_for_c<Factories_T &&, Ts> && ...)
    constexpr explicit StackedTuple(Factories_T&&... factories);

    template <size_t index_T, typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&) noexcept -> ::util::meta::
        forward_like_t<util::meta::type_list_at_t<util::TypeList<Ts...>, index_T>, Self_T>;

    template <::util::meta::storable_c T, typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&) noexcept -> ::util::meta::forward_like_t<T, Self_T>;

private:
    Impl<std::make_index_sequence<sizeof...(Ts)>, Ts...> m_impl;
};

}   // namespace util

template <template <typename T_, T_...> typename IntegerSequence_T>
constexpr Impl<IntegerSequence_T<size_t>>::Impl(std::tuple<>)
{}

template <typename>
struct gather_dependencies_helper;

template <template <typename...> typename TypeList_T, typename... SelectedTypes_T>
struct gather_dependencies_helper<TypeList_T<SelectedTypes_T...>> {
    template <typename... Ts>
    [[nodiscard]]
    constexpr static auto operator()(std::tuple<Ts...>& tuple)
        -> std::tuple<SelectedTypes_T...>
    {
        return { std::forward_like<SelectedTypes_T>(
            std::get<std::remove_cvref_t<SelectedTypes_T>&>(tuple)
        )... };
    }
};

template <typename Callable_T, typename... Ts>
constexpr auto gather_dependencies(std::tuple<Ts...>& stack)
{
    using namespace std::literals::string_literals;

    using RequiredResourcesTuple_T = util::meta::arguments_of_t<Callable_T>;

    util::meta::for_each<RequiredResourcesTuple_T>([]<typename Dependency_T> {
        static_assert(
            util::meta::type_list_contains_v<
                util::TypeList<Ts...>,
                std::remove_cvref_t<Dependency_T>&>,
            // TODO: constexpr std::format
            "Dependency `"s + util::meta::name_of<std::remove_cvref_t<Dependency_T>>()
                + "` not found for `" + util::meta::name_of<Callable_T>() + "`"
        );
    });

    return gather_dependencies_helper<RequiredResourcesTuple_T>::operator()(stack);
}

template <template <typename T_, T_...> typename IntegerSequence_T, size_t I, typename T>
template <typename... Stacked_T, typename Factory_T>
constexpr Impl<IntegerSequence_T<size_t, I>, T>::Impl(
    std::tuple<Stacked_T&...> stack,
    Factory_T&&               factory
)
    : Leaf<I, T>{ .value{ std::apply(
          std::forward<Factory_T>(factory),
          gather_dependencies<Factory_T>(stack)
      ) } }
{}

template <
    template <typename T_, T_...> typename IntegerSequence_T,
    size_t I,
    size_t... Is,
    typename T,
    typename... Ts>
template <typename... Stacked_T, typename Factory_T, typename... Factories_T>
constexpr Impl<IntegerSequence_T<size_t, I, Is...>, T, Ts...>::Impl(
    std::tuple<Stacked_T&...> stack,
    Factory_T&&               factory,
    Factories_T&&... factories
)
    : Leaf<I, T>{ .value{ std::apply(
          std::forward<Factory_T>(factory),
          gather_dependencies<Factory_T>(stack)
      ) } },
      Impl<IntegerSequence_T<size_t, Is...>, Ts...>{
          std::tuple_cat(stack, std::tuple<T&>{ Leaf<I, T>::value }),
          std::forward<Factories_T>(factories)...
      }
{}

template <::util::meta::storable_c... Ts>
template <typename... Factories_T>
    requires(factory_for_c<Factories_T &&, Ts> && ...)
constexpr util::StackedTuple<Ts...>::StackedTuple(Factories_T&&... factories)
    : m_impl{ std::tuple<>{}, std::forward<Factories_T>(factories)... }
{}

template <::util::meta::storable_c... Ts>
template <size_t index_T, typename Self_T>
constexpr auto util::StackedTuple<Ts...>::get(this Self_T&& self) noexcept
    -> meta::forward_like_t<meta::type_list_at_t<TypeList<Ts...>, index_T>, Self_T>
{
    return std::forward_like<Self_T>(
        self.m_impl.Leaf<index_T, meta::type_list_at_t<TypeList<Ts...>, index_T>>::value
    );
}

template <::util::meta::storable_c... Ts>
template <::util::meta::storable_c T, typename Self_T>
constexpr auto util::StackedTuple<Ts...>::get(this Self_T&& self) noexcept
    -> meta::forward_like_t<T, Self_T>
{
    return std::forward_like<Self_T>(
        self.m_impl.Leaf<meta::type_list_index_of_v<util::TypeList<Ts...>, T>, T>::value
    );
}
