module;

#include <array>
#include <type_traits>

export module utility.ValueSequence;

import utility.meta.type_traits.all_same;

namespace util {

export template <typename T, T... values_T>
struct ValueSequence {
    template <typename Visitor>
    constexpr static auto for_each(Visitor&& visitor) -> void;

    template <typename IndexedVisitor>
    constexpr static auto enumerate(IndexedVisitor&& visitor) -> void;

    [[nodiscard]]
    constexpr static auto realize() noexcept(std::is_nothrow_copy_constructible_v<T>)
        -> std::array<T, sizeof...(values_T)>;
};

}   // namespace util

template <typename T, T... values_T>
template <typename Visitor>
constexpr auto util::ValueSequence<T, values_T...>::for_each(Visitor&& visitor) -> void
{
    [&visitor]<size_t... Is>(std::index_sequence<Is...>) {
        (visitor.template operator()<values_T...[Is]>(), ...);
    }(std::make_index_sequence<sizeof...(values_T)>{});
}

template <typename T, T... values_T>
template <typename IndexedVisitor>
constexpr auto util::ValueSequence<T, values_T...>::enumerate(IndexedVisitor&& visitor)
    -> void
{
    [&visitor]<size_t... Is>(std::index_sequence<Is...>) {
        (visitor.template operator()<Is, values_T...[Is]>(), ...);
    }(std::make_index_sequence<sizeof...(values_T)>{});
}

template <typename T, T... values_T>
constexpr auto util::ValueSequence<T, values_T...>::realize(
) noexcept(std::is_nothrow_copy_constructible_v<T>) -> std::array<T, sizeof...(values_T)>
{
    return std::array<T, sizeof...(values_T)>{ values_T... };
}
