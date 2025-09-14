module;

#include <concepts>
#include <type_traits>
#include <utility>

export module ddge.utility.containers.Strong;

import ddge.utility.meta.type_traits.forward_like;

struct StrongBase {
    auto operator==(const StrongBase&) const -> bool = default;
    auto operator<=>(const StrongBase&) const        = default;
};

namespace ddge::util {

export template <typename T, typename Tag_T>
class Strong : public StrongBase {
public:
    using Underlying = T;

    Strong()
        requires std::default_initializable<T>
    = default;

    template <typename... Args_T>
    constexpr explicit Strong(
        std::in_place_t,
        Args_T&&... args
    ) noexcept(std::is_nothrow_constructible_v<T, Args_T...>);

    template <typename U>
        requires std::constructible_from<T, U&&>
    constexpr explicit Strong(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>);

    auto operator==(const Strong&) const -> bool = default;
    auto operator<=>(const Strong&) const        = default;

    template <typename Self_T>
    constexpr auto underlying(this Self_T&&) noexcept -> meta::forward_like_t<T, Self_T>;

private:
    T m_value{};
};

}   // namespace ddge::util

export template <std::derived_from<StrongBase> Strong_T>
    requires requires(typename Strong_T::Underlying underlying) {
        std::hash<decltype(underlying)>{}(underlying);
    }
struct std::hash<Strong_T> {
    template <typename UStrong_T>
        requires std::same_as<std::remove_cvref_t<UStrong_T>, Strong_T>
    [[nodiscard]]
    constexpr static auto operator()(UStrong_T&& strong
    ) noexcept(noexcept(std::hash<typename Strong_T::Underlying>{}(strong.underlying())))
        -> std::size_t
    {
        return std::hash<std::decay_t<decltype(strong.underlying())>>{}(strong.underlying(
        ));
    }
};

template <typename T, typename Tag_T>
template <typename... Args_T>
constexpr ddge::util::Strong<T, Tag_T>::Strong(
    std::in_place_t,
    Args_T&&... args
) noexcept(std::is_nothrow_constructible_v<T, Args_T...>)
    : m_value(std::forward<Args_T>(args)...)
{}

template <typename T, typename Tag_T>
template <typename U>
    requires std::constructible_from<T, U&&>
constexpr ddge::util::Strong<T, Tag_T>::Strong(
    U&& value
) noexcept(std::is_nothrow_constructible_v<T, U>)
    : m_value{ std::forward<U>(value) }
{}

template <typename T, typename Tag_T>
template <typename Self_T>
constexpr auto ddge::util::Strong<T, Tag_T>::underlying(this Self_T&& self) noexcept
    -> meta::forward_like_t<T, Self_T>
{
    return std::forward_like<Self_T>(self.m_value);
}
