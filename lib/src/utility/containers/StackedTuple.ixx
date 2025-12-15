module;

#include <concepts>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <fmt/compile.h>

export module ddge.utility.containers.StackedTuple;

import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.concepts.integer_sequence.index_sequence;
import ddge.utility.meta.concepts.storable;
import ddge.utility.meta.reflection.name_of;
import ddge.utility.meta.type_traits.forward_like;
import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.functional.result_of;
import ddge.utility.meta.type_traits.type_list.type_list_at;
import ddge.utility.meta.type_traits.type_list.type_list_contains;
import ddge.utility.meta.type_traits.type_list.type_list_index_of;
import ddge.utility.TypeList;

template <std::size_t, typename T>
struct Leaf {
    T value;
};

template <typename Factory_T, typename T>
concept factory_for_c =
    std::constructible_from<T, ddge::util::meta::result_of_t<Factory_T>>;

template <ddge::util::meta::index_sequence_c, typename... Ts>
struct Impl;

template <template <typename T_, T_...> typename IntegerSequence_T>
struct Impl<IntegerSequence_T<std::size_t>> {
    constexpr explicit Impl(std::tuple<>);
};

template <template <typename T_, T_...> typename IntegerSequence_T, std::size_t I, typename T>
struct Impl<IntegerSequence_T<std::size_t, I>, T> : Leaf<I, T> {
    template <typename... Stacked_T, typename Factory_T>
    constexpr Impl(std::tuple<Stacked_T&...> stack, Factory_T&& factory);
};

template <
    template <typename T_, T_...> typename IntegerSequence_T,
    std::size_t I,
    std::size_t J,
    std::size_t... Is,
    typename T,
    typename U,
    typename... Ts>
struct Impl<IntegerSequence_T<std::size_t, I, J, Is...>, T, U, Ts...>
    : Leaf<I, T>, Impl<IntegerSequence_T<std::size_t, J, Is...>, U, Ts...> {
    template <typename... Stacked_T, typename Factory_T, typename... Factories_T>
    constexpr Impl(
        std::tuple<Stacked_T&...> stack,
        Factory_T&&               factory,
        Factories_T&&... factories
    );
};

namespace ddge::util {

export template <meta::storable_c... Ts>
class StackedTuple {
public:
    template <typename... Factories_T>
        requires(factory_for_c<Factories_T &&, Ts> && ...)
    constexpr explicit StackedTuple(Factories_T&&... factories);

    template <std::size_t index_T, typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&) noexcept -> meta::
        forward_like_t<meta::type_list_at_t<util::TypeList<Ts...>, index_T>, Self_T>;

    template <meta::storable_c T, typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&) noexcept -> meta::forward_like_t<T, Self_T>;

private:
    Impl<std::make_index_sequence<sizeof...(Ts)>, Ts...> m_impl;
};

}   // namespace ddge::util

template <template <typename T_, T_...> typename IntegerSequence_T>
constexpr Impl<IntegerSequence_T<std::size_t>>::Impl(std::tuple<>)
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
    using namespace fmt::literals;

    using RequiredResourcesTuple_T = ddge::util::meta::arguments_of_t<Callable_T>;

    ddge::util::meta::for_each<RequiredResourcesTuple_T>([]<typename Dependency_T> {
        static_assert(
            ddge::util::meta::type_list_contains_v<
                ddge::util::TypeList<Ts...>,
                std::remove_cvref_t<Dependency_T>&>,
            // TODO: constexpr std::format
            fmt::format(
                "Dependency `{}` not found for `{}`"_cf,
                ddge::util::meta::name_of<std::remove_cvref_t<Dependency_T>>(),
                ddge::util::meta::name_of<Callable_T>()
            )
        );
    });

    return gather_dependencies_helper<RequiredResourcesTuple_T>::operator()(stack);
}

template <template <typename T_, T_...> typename IntegerSequence_T, std::size_t I, typename T>
template <typename... Stacked_T, typename Factory_T>
constexpr Impl<IntegerSequence_T<std::size_t, I>, T>::Impl(
    std::tuple<Stacked_T&...> stack,
    Factory_T&&               factory
)
    : Leaf<I, T>{ .value{ std::apply(
          std::forward<Factory_T>(factory),
          gather_dependencies<Factory_T>(stack)
      ) } }
{}

template <
    template <typename T_, T_...> class IntegerSequence_T,
    std::size_t I,
    std::size_t J,
    std::size_t... Is,
    typename T,
    typename U,
    typename... Ts>
template <typename... Stacked_T, typename Factory_T, typename... Factories_T>
constexpr ::Impl<IntegerSequence_T<std::size_t, I, J, Is...>, T, U, Ts...>::Impl(
    std::tuple<Stacked_T&...> stack,
    Factory_T&&               factory,
    Factories_T&&... factories
)
    : Leaf<I, T>{ .value{ std::apply(
          std::forward<Factory_T>(factory),
          gather_dependencies<Factory_T>(stack)
      ) } },
      Impl<IntegerSequence_T<std::size_t, J, Is...>, U, Ts...>{
          std::tuple_cat(stack, std::tuple<T&>{ Leaf<I, T>::value }),
          std::forward<Factories_T>(factories)...
      }
{}

template <ddge::util::meta::storable_c... Ts>
template <typename... Factories_T>
    requires(factory_for_c<Factories_T &&, Ts> && ...)
constexpr ddge::util::StackedTuple<Ts...>::StackedTuple(Factories_T&&... factories)
    : m_impl{ std::tuple<>{}, std::forward<Factories_T>(factories)... }
{}

template <ddge::util::meta::storable_c... Ts>
template <std::size_t index_T, typename Self_T>
constexpr auto ddge::util::StackedTuple<Ts...>::get(this Self_T&& self) noexcept
    -> meta::forward_like_t<meta::type_list_at_t<TypeList<Ts...>, index_T>, Self_T>
{
    return std::forward_like<Self_T>(
        self.m_impl.Leaf<index_T, meta::type_list_at_t<TypeList<Ts...>, index_T>>::value
    );
}

template <ddge::util::meta::storable_c... Ts>
template <ddge::util::meta::storable_c T, typename Self_T>
constexpr auto ddge::util::StackedTuple<Ts...>::get(this Self_T&& self) noexcept
    -> meta::forward_like_t<T, Self_T>
{
    return std::forward_like<Self_T>(
        self.m_impl.Leaf<meta::type_list_index_of_v<util::TypeList<Ts...>, T>, T>::value
    );
}
