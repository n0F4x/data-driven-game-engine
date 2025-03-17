module;

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#ifdef ENGINE_ENABLE_STATIC_TESTS
  #include <cassert>
#endif

export module utility.containers.StackedTuple;

import utility.meta.concepts.decayed;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.invoke_result_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

template <typename T>
struct Leaf {
    T value;
};

template <typename T, typename Resource_T>
concept decays_to_factory_c = std::constructible_from<
    Resource_T,
    util::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<T>>>>;

template <typename... Ts>
struct Impl;

template <>
struct Impl<> {
    constexpr explicit Impl(std::tuple<>);
};

template <typename T>
struct Impl<T> : Leaf<T> {
    template <typename... Stacked_T, typename Factory_T>
    constexpr Impl(std::tuple<Stacked_T&...>&& stack, Factory_T&& factory);
};

template <typename T, typename... Ts>
struct Impl<T, Ts...> : Leaf<T>, Impl<Ts...> {
    template <typename... Stacked_T, typename Factory_T, typename... Factories_T>
    constexpr Impl(
        std::tuple<Stacked_T&...>&& stack,
        Factory_T&&                 factory,
        Factories_T&&... factories
    );
};

namespace util {

export template <::util::meta::decayed_c... Ts>
class StackedTuple {
public:
    template <::decays_to_factory_c<Ts>... Factories_T>
    constexpr explicit StackedTuple(Factories_T&&... factories);
    StackedTuple(const StackedTuple&) = delete;

    template <::util::meta::decayed_c T, typename Self_T>
        requires(::util::meta::type_list_contains_v<::util::TypeList<Ts...>, T>)
    [[nodiscard]]
    constexpr auto get(this Self_T&&) noexcept -> ::util::meta::forward_like_t<T, Self_T>;

private:
    Impl<Ts...> m_impl;
};

}   // namespace util

constexpr Impl<>::Impl(std::tuple<>) {}

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
            std::get<std::decay_t<SelectedTypes_T>&>(tuple)
        )... };
    }
};

template <typename Callable_T, typename... Ts>
constexpr auto gather_dependencies(std::tuple<Ts...>& stack)
{
    using RequiredResourcesTuple_T = util::meta::arguments_of_t<Callable_T>;

    return gather_dependencies_helper<RequiredResourcesTuple_T>::operator()(stack);
}

template <typename T>
template <typename... Stacked_T, typename Factory_T>
constexpr Impl<T>::Impl(std::tuple<Stacked_T&...>&& stack, Factory_T&& factory)
    : Leaf<T>{ std::apply(
          std::forward<Factory_T>(factory),
          gather_dependencies<std::remove_pointer_t<std::decay_t<Factory_T>>>(stack)
      ) }
{}

template <typename T, typename... Ts>
template <typename... Stacked_T, typename Factory_T, typename... Factories_T>
constexpr Impl<T, Ts...>::Impl(
    std::tuple<Stacked_T&...>&& stack,
    Factory_T&&                 factory,
    Factories_T&&... factories
)
    : Leaf<T>{ std::apply(
          std::forward<Factory_T>(factory),
          gather_dependencies<std::remove_pointer_t<std::decay_t<Factory_T>>>(stack)
      ) },
      Impl<Ts...>{ std::tuple_cat(std::move(stack), std::tuple<T&>{ Leaf<T>::value }),
                   std::forward<Factories_T>(factories)... }
{}

template <::util::meta::decayed_c... Ts>
template <::decays_to_factory_c<Ts>... Factories_T>
constexpr util::StackedTuple<Ts...>::StackedTuple(Factories_T&&... factories)
    : m_impl{ std::tuple<>{}, std::forward<Factories_T>(factories)... }
{}

template <::util::meta::decayed_c... Ts>
template <::util::meta::decayed_c T, typename Self_T>
    requires(::util::meta::type_list_contains_v<::util::TypeList<Ts...>, T>)
constexpr auto util::StackedTuple<Ts...>::get(this Self_T&& self) noexcept
    -> meta::forward_like_t<T, Self_T>
{
    return std::forward_like<Self_T>(self.m_impl.Leaf<T>::value);
}

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

namespace {

struct First {
    int value{ 42 };
};

struct Second {
    std::reference_wrapper<const int> ref;
};

[[nodiscard]]
constexpr auto make_first() -> First
{
    return First{};
}

[[nodiscard]]
constexpr auto make_second_fn(First& first) -> Second
{
    return Second{ .ref = first.value };
}

auto make_second_lambda{ [] [[nodiscard]]
                         (const First& first) -> Second {
                             return Second{ .ref = first.value };
                         } };

struct MakeSecondFunctor {
    [[nodiscard]]
    constexpr auto operator()(First& first) const -> Second
    {
        return Second{ .ref = first.value };
    }
};

}   // namespace

static_assert(
    [] {
        util::StackedTuple<>{};

        return true;
    }(),
    "empty creation test failed"
);

static_assert(
    [] {
        util::StackedTuple<First, Second> stacked_tuple{ make_first, make_second_fn };

        assert(stacked_tuple.get<First>().value == stacked_tuple.get<Second>().ref.get());

        return true;
    }(),
    "inject with function pointer test failed"
);

static_assert(
    [] {
        util::StackedTuple<First, Second> stacked_tuple{ make_first, make_second_lambda };

        assert(stacked_tuple.get<First>().value == stacked_tuple.get<Second>().ref.get());

        return true;
    }(),
    "inject with lambda test failed"
);

static_assert(
    [] {
        util::StackedTuple<First, Second> stacked_tuple{ make_first, MakeSecondFunctor{} };

        assert(stacked_tuple.get<First>().value == stacked_tuple.get<Second>().ref.get());

        return true;
    }(),
    "inject with functor test failed"
);

#endif
