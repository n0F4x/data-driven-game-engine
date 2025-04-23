module;

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#ifdef ENGINE_ENABLE_STATIC_TESTS
  #include <cassert>
  #include <memory>
#endif

export module utility.containers.StackedTuple;

import utility.meta.concepts.decayed;
import utility.meta.concepts.integer_sequence.index_sequence;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.meta.type_traits.type_list.type_list_at;
import utility.meta.type_traits.type_list.type_list_index_of;
import utility.TypeList;

template <size_t, typename T>
struct Leaf {
    T value;
};

template <typename T, typename Resource_T>
concept decays_to_factory_c =
    std::constructible_from<Resource_T, util::meta::result_of_t<std::decay_t<T>>>;

template <util::meta::index_sequence_c, typename... Ts>
struct Impl;

template <template <typename T_, T_...> typename IntegerSequence_T>
struct Impl<IntegerSequence_T<size_t>> {
    constexpr explicit Impl(std::tuple<>);
};

template <template <typename T_, T_...> typename IntegerSequence_T, size_t I, typename T>
struct Impl<IntegerSequence_T<size_t, I>, T> : Leaf<I, T> {
    template <typename... Stacked_T, typename Factory_T>
    constexpr Impl(std::tuple<Stacked_T&...>&& stack, Factory_T&& factory);
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

    template <size_t index_T, typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&) noexcept -> ::util::meta::
        forward_like_t<util::meta::type_list_at_t<util::TypeList<Ts...>, index_T>, Self_T>;

    template <::util::meta::decayed_c T, typename Self_T>
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

template <template <typename T_, T_...> typename IntegerSequence_T, size_t I, typename T>
template <typename... Stacked_T, typename Factory_T>
constexpr Impl<IntegerSequence_T<size_t, I>, T>::Impl(
    std::tuple<Stacked_T&...>&& stack,
    Factory_T&&                 factory
)
    : Leaf<I, T>{ std::apply(
          std::forward<Factory_T>(factory),
          gather_dependencies<std::decay_t<Factory_T>>(stack)
      ) }
{}

template <
    template <typename T_, T_...> typename IntegerSequence_T,
    size_t I,
    size_t... Is,
    typename T,
    typename... Ts>
template <typename... Stacked_T, typename Factory_T, typename... Factories_T>
constexpr Impl<IntegerSequence_T<size_t, I, Is...>, T, Ts...>::Impl(
    std::tuple<Stacked_T&...>&& stack,
    Factory_T&&                 factory,
    Factories_T&&... factories
)
    : Leaf<I, T>{ std::apply(
          std::forward<Factory_T>(factory),
          gather_dependencies<std::remove_pointer_t<std::decay_t<Factory_T>>>(stack)
      ) },
      Impl<IntegerSequence_T<size_t, Is...>, Ts...>{
          std::tuple_cat(std::move(stack), std::tuple<T&>{ Leaf<I, T>::value }),
          std::forward<Factories_T>(factories)...
      }
{}

template <::util::meta::decayed_c... Ts>
template <::decays_to_factory_c<Ts>... Factories_T>
constexpr util::StackedTuple<Ts...>::StackedTuple(Factories_T&&... factories)
    : m_impl{ std::tuple<>{}, std::forward<Factories_T>(factories)... }
{}

template <::util::meta::decayed_c... Ts>
template <size_t index_T, typename Self_T>
constexpr auto util::StackedTuple<Ts...>::get(this Self_T&& self) noexcept
    -> meta::forward_like_t<meta::type_list_at_t<TypeList<Ts...>, index_T>, Self_T>
{
    return std::forward_like<Self_T>(
        self.m_impl.Leaf<index_T, meta::type_list_at_t<TypeList<Ts...>, index_T>>::value
    );
}

template <::util::meta::decayed_c... Ts>
template <::util::meta::decayed_c T, typename Self_T>
constexpr auto util::StackedTuple<Ts...>::get(this Self_T&& self) noexcept
    -> meta::forward_like_t<T, Self_T>
{
    return std::forward_like<Self_T>(
        self.m_impl.Leaf<meta::type_list_index_of_v<util::TypeList<Ts...>, T>, T>::value
    );
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

static_assert(
    [] {
        std::vector<int> order;
        order.reserve(3);

        struct OrderWriter {
            std::reference_wrapper<std::vector<int>> order;
            int                                      number;

            constexpr ~OrderWriter()
            {
                order.get().push_back(number);
            }
        };

        auto stacked_tuple =
            std::make_unique<util::StackedTuple<OrderWriter, OrderWriter, OrderWriter>>(
                [&order] { return OrderWriter{ .order = order, .number = 0 }; },
                [&order] { return OrderWriter{ .order = order, .number = 1 }; },
                [&order] { return OrderWriter{ .order = order, .number = 2 }; }
            );

        stacked_tuple.reset();

        const std::vector expected{ 2, 1, 0 };
        assert(order == expected);

        return true;
    }(),
    "destruction order test failed"
);

#endif
