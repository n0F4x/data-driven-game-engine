module;

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

export module ddge.utility.containers.Tuple;

import ddge.utility.meta.concepts.contained_exactly_once;
import ddge.utility.meta.type_traits.forward_like;

namespace ddge::util {

export template <typename... Ts>
class Tuple;

namespace internal {

template <typename T, typename... Ts, std::size_t... indices_T>
[[nodiscard]]
consteval auto index_of(std::index_sequence<indices_T...>) -> std::size_t
{
    std::size_t result;
    ((std::is_same_v<T, Ts> ? result = indices_T : 0), ...);
    return result;
}

template <typename T, typename... Ts>
inline constexpr std::size_t index_of_v{
    index_of<T, Ts...>(std::make_index_sequence<sizeof...(Ts)>{})
};

class TupleBase {};

template <std::size_t, typename T>
struct Node {
    T data{};
};

template <typename IndexSequence_T, typename... Ts>
class TupleImpl;

template <std::size_t... indices_T, typename... Ts>
class TupleImpl<std::index_sequence<indices_T...>, Ts...>
    : private Node<indices_T, Ts>...   //
{
    template <typename, typename...>
    friend class TupleImpl;

public:
    explicit TupleImpl() = default;

    template <typename... Us>
        requires(sizeof...(Us) != 0) && (sizeof...(Us) == sizeof...(Ts))
             && (sizeof...(Us) != 1
                 || !std::same_as<std::remove_cvref_t<Us...[0]>, TupleImpl>)
             && (std::constructible_from<Ts, Us &&> && ...)
    explicit constexpr TupleImpl(Us&&... values)
        : Node<indices_T, Ts>{ std::forward<Us>(values) }...
    {}

    template <class... Us>
        requires(sizeof...(Ts) == sizeof...(Us))
    constexpr auto operator==(const Tuple<Us...>& other) const -> bool
    {
        return (
            (Node<indices_T, Ts>::data == other.template Node<indices_T, Us>::data) && ...
        );
    }

    template <class... Us>
        requires(sizeof...(Ts) == sizeof...(Us))
    constexpr auto operator<=>(const Tuple<Us...>& other) const -> bool
    {
        return (
            (Node<indices_T, Ts>::data <=> other.template Node<indices_T, Us>::data) && ...
        );
    }

    template <std::size_t index_T, typename Self_T>
            requires(index_T < sizeof...(Ts))
                 && std::derived_from<std::remove_cvref_t<Self_T>, TupleImpl>
        [[nodiscard]]
        constexpr auto get(this Self_T&& self) noexcept
            -> meta::forward_like_t<Ts... [index_T], Self_T>
        {
            return std::forward_like<Self_T>(
                self.internal::template Node<index_T, Ts...[index_T]>::data
            );
        }

        template <typename T, typename Self_T>
            requires meta::contained_exactly_once_c<T, Ts...> && std::derived_from<std::remove_cvref_t<Self_T>, TupleImpl>
        [[nodiscard]]
        constexpr auto get(this Self_T&& self) noexcept
            -> meta::forward_like_t<T, Self_T>
    {
        constexpr static std::size_t index{ index_of_v<T, Ts...> };
        return std::forward_like<Self_T>(
            self.internal::template Node<index, Ts...[index]>::data
        );
    }

    template <typename F, typename Self_T>
        requires std::derived_from<std::remove_cvref_t<Self_T>, TupleImpl>
    constexpr auto apply(this Self_T&& self, F&& func)
    {
        return std::invoke(
            std::forward<F>(func),
            std::forward_like<Self_T>(self.internal::template Node<indices_T, Ts>::data)...
        );
    }
};

}   // namespace internal

template <typename... Ts>
class Tuple final
    : public internal::TupleBase,
      public internal::TupleImpl<std::make_index_sequence<sizeof...(Ts)>, Ts...>   //
{
    template <typename, typename...>
    friend class TupleImpl;

public:
    using internal::TupleImpl<std::make_index_sequence<sizeof...(Ts)>, Ts...>::TupleImpl;

    using internal::TupleImpl<std::make_index_sequence<sizeof...(Ts)>, Ts...>::operator==;
    using internal::TupleImpl<std::make_index_sequence<sizeof...(Ts)>, Ts...>::operator<=>;
};

template <class... Ts>
Tuple(Ts...) -> Tuple<Ts...>;

export template <std::size_t index_T, typename... Ts>
    requires(index_T < sizeof...(Ts))
            [[nodiscard]]
            constexpr auto get(Tuple<Ts...>& tuple) noexcept -> Ts... [index_T]
          & { return tuple.template get<index_T>(); }

            export template <std::size_t index_T, typename... Ts>
                requires(index_T < sizeof...(Ts))
[[nodiscard]] constexpr auto get(Tuple<Ts...>&& tuple) noexcept -> Ts...[index_T]&&
{
    return std::move(tuple).template get<index_T>();
}

export template <std::size_t index_T, typename... Ts>
    requires(index_T < sizeof...(Ts))
            [[nodiscard]]
            constexpr auto get(const Tuple<Ts...>& tuple) noexcept -> const Ts...
            [index_T]
          & { return tuple.template get<index_T>(); }

            export template <std::size_t index_T, typename... Ts>
                requires(index_T < sizeof...(Ts))
[[nodiscard]] constexpr auto get(const Tuple<Ts...>&& tuple) noexcept -> const Ts
    ...[index_T]&&
{
    return std::move(tuple).template get<index_T>();
}

export template <typename T, typename... Ts>
    requires meta::contained_exactly_once_c<T, Ts...>
[[nodiscard]]
constexpr auto get(Tuple<Ts...>& tuple) noexcept -> T&
{
    return tuple.template get<T>();
}

export template <typename T, typename... Ts>
    requires meta::contained_exactly_once_c<T, Ts...>
[[nodiscard]]
constexpr auto get(Tuple<Ts...>&& tuple) noexcept -> T&&
{
    return std::move(tuple).template get<T>();
}

export template <typename T, typename... Ts>
    requires meta::contained_exactly_once_c<T, Ts...>
[[nodiscard]]
constexpr auto get(const Tuple<Ts...>& tuple) noexcept -> const T&
{
    return tuple.template get<T>();
}

export template <typename T, typename... Ts>
    requires meta::contained_exactly_once_c<T, Ts...>
[[nodiscard]]
constexpr auto get(const Tuple<Ts...>&& tuple) noexcept -> const T&&
{
    return std::move(tuple).template get<T>();
}

export template <typename F, typename Tuple_T>
    requires std::derived_from<std::remove_cvref_t<Tuple_T>, internal::TupleBase>
constexpr auto apply(F&& func, Tuple_T&& tuple)
{
    return std::forward<Tuple_T>(tuple).apply(std::forward<F>(func));
}

}   // namespace ddge::util

template <typename... Ts>
struct std::tuple_size<ddge::util::Tuple<Ts...>> {
    constexpr static std::size_t value{ sizeof...(Ts) };
};

template <std::size_t index_T, typename... Ts>
struct std::tuple_element<index_T, ddge::util::Tuple<Ts...>> {
    using type = Ts...[index_T];
};
