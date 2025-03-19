module;

#include <concepts>
#include <type_traits>
#include <utility>

export module utility.Strong;

import utility.meta.type_traits.forward_like;

struct real_impl_t {};

constexpr real_impl_t real_impl{};

namespace util {

export template <typename T, typename OnlyFriend_T = void, auto tag_T = [] {}>
class Strong {
public:
    using Underlying = T;

    Strong()
        requires std::is_void_v<OnlyFriend_T> && std::default_initializable<T>
    = default;

    template <typename... Args_T>
    constexpr explicit Strong(
        std::in_place_t,
        Args_T&&... args
    ) noexcept(std::is_nothrow_constructible_v<T, Args_T...>)
        requires std::is_void_v<OnlyFriend_T>;

    template <typename U>
        requires std::same_as<std::remove_cvref_t<U>, T>
    constexpr explicit Strong(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
        requires std::is_void_v<OnlyFriend_T>;

    constexpr auto operator<=>(const Strong&) const = default;

    template <typename Self_T>
    constexpr auto underlying(this Self_T&&) noexcept
        -> ::util::meta::forward_like_t<T, Self_T>;

private:
    friend OnlyFriend_T;

    T m_value{};

    Strong()
        requires(!std::is_void_v<OnlyFriend_T>) && std::default_initializable<T>
    = default;

    template <typename... Args_T>
    constexpr explicit Strong(
        std::in_place_t,
        Args_T&&... args
    ) noexcept(std::is_nothrow_constructible_v<T, Args_T...>)
        requires(!std::is_void_v<OnlyFriend_T>);

    template <typename U>
        requires std::same_as<std::remove_cvref_t<U>, T>
    constexpr explicit Strong(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
        requires(!std::is_void_v<OnlyFriend_T>);

    template <typename... Args_T>
    constexpr explicit Strong(
        real_impl_t,
        std::in_place_t,
        Args_T&&... args
    ) noexcept(std::is_nothrow_constructible_v<T, Args_T...>);

    template <typename U>
        requires std::same_as<std::remove_cvref_t<U>, T>
    constexpr explicit Strong(
        real_impl_t,
        U&& value
    ) noexcept(std::is_nothrow_constructible_v<T, U>);
};

export template <typename>
struct is_specialization_of_strong : std::false_type {};

template <typename T, typename OnlyFriend_T, auto tag_T>
struct is_specialization_of_strong<util::Strong<T, OnlyFriend_T, tag_T>>
    : std::true_type {};

export template <typename T>
concept specialization_of_strong_c = is_specialization_of_strong<T>::value;

}   // namespace util

export template <typename T, typename OnlyFriend_T, auto tag_T>
struct std::hash<util::Strong<T, OnlyFriend_T, tag_T>> {
    template <typename Strong_T>
        requires std::
            same_as<std::remove_cvref_t<Strong_T>, util::Strong<T, OnlyFriend_T, tag_T>>
        [[nodiscard]]
        auto operator()(Strong_T&& strong
        ) const noexcept(requires(std::hash<T> underlying_hasher, T underlying) {
            underlying_hasher(
                static_cast<util::meta::forward_like_t<T, Strong_T>>(underlying)
            );
        }) -> size_t
    {
        return std::hash<T>{}(strong.underlying());
    }
};

template <typename T, typename OnlyFriend_T, auto tag_T>
template <typename... Args_T>
constexpr util::Strong<T, OnlyFriend_T, tag_T>::Strong(
    std::in_place_t,
    Args_T&&... args
) noexcept(std::is_nothrow_constructible_v<T, Args_T...>)
    requires std::is_void_v<OnlyFriend_T>
    : Strong{ real_impl, std::in_place, std::forward<Args_T>(args)... }
{}

template <typename T, typename OnlyFriend_T, auto tag_T>
template <typename U>
    requires std::same_as<std::remove_cvref_t<U>, T>
constexpr util::Strong<T, OnlyFriend_T, tag_T>::Strong(
    U&& value
) noexcept(std::is_nothrow_constructible_v<T, U>)
    requires std::is_void_v<OnlyFriend_T>
    : Strong{ real_impl, std::forward<U>(value) }
{}

template <typename T, typename OnlyFriend_T, auto tag_T>
template <typename Self_T>
constexpr auto util::Strong<T, OnlyFriend_T, tag_T>::underlying(
    this Self_T&& self
) noexcept -> meta::forward_like_t<T, Self_T>
{
    return std::forward_like<Self_T>(self.m_value);
}

template <typename T, typename OnlyFriend_T, auto tag_T>
template <typename... Args_T>
constexpr util::Strong<T, OnlyFriend_T, tag_T>::Strong(
    std::in_place_t,
    Args_T&&... args
) noexcept(std::is_nothrow_constructible_v<T, Args_T...>)
    requires(!std::is_void_v<OnlyFriend_T>)
    : Strong{ real_impl, std::in_place, std::forward<Args_T>(args)... }
{}

template <typename T, typename OnlyFriend_T, auto tag_T>
template <typename U>
    requires std::same_as<std::remove_cvref_t<U>, T>
constexpr util::Strong<T, OnlyFriend_T, tag_T>::Strong(
    U&& value
) noexcept(std::is_nothrow_constructible_v<T, U>)
    requires(!std::is_void_v<OnlyFriend_T>)
    : Strong{ real_impl, std::forward<U>(value) }
{}

template <typename T, typename OnlyFriend_T, auto tag_T>
template <typename... Args_T>
constexpr util::Strong<T, OnlyFriend_T, tag_T>::Strong(
    real_impl_t,
    std::in_place_t,
    Args_T&&... args
) noexcept(std::is_nothrow_constructible_v<T, Args_T...>)
    : m_value(std::forward<Args_T>(args)...)
{}

template <typename T, typename OnlyFriend_T, auto tag_T>
template <typename U>
    requires std::same_as<std::remove_cvref_t<U>, T>
constexpr util::Strong<T, OnlyFriend_T, tag_T>::Strong(
    real_impl_t,
    U&& value
) noexcept(std::is_nothrow_constructible_v<T, U>)
    : m_value{ std::forward<U>(value) }
{}

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(!std::is_same_v<util::Strong<int>, util::Strong<int>>);

#endif
