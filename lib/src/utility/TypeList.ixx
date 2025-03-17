module;

#include <utility>

export module utility.TypeList;

namespace util {

export template <typename... Ts>
struct TypeList {
    template <typename Visitor_T>
    constexpr static auto for_each(Visitor_T&& visitor) -> void;

    template <typename IndexedVisitor_T>
    constexpr static auto enumerate(IndexedVisitor_T&& visitor) -> void;
};

}   // namespace util

template <typename... Ts>
template <typename Visitor_T>
constexpr auto util::TypeList<Ts...>::for_each(Visitor_T&& visitor) -> void
{
    [&visitor]<size_t... indices_T>(std::index_sequence<indices_T...>) {
        (visitor.template operator()<Ts...[indices_T]>(), ...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
}

template <typename... Ts>
template <typename IndexedVisitor_T>
constexpr auto util::TypeList<Ts...>::enumerate(IndexedVisitor_T&& visitor) -> void
{
    [&visitor]<size_t... indices_T>(std::index_sequence<indices_T...>) {
        (visitor.template operator()<indices_T, Ts...[indices_T]>(), ...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
}
