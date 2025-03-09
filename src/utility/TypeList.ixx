module;

#include <utility>

export module utility.TypeList;

namespace util {

export template <typename... Ts>
struct TypeList {
    template <typename Visitor>
    constexpr static auto for_each(Visitor&& visitor) -> void;

    template <typename IndexedVisitor>
    constexpr static auto enumerate(IndexedVisitor&& visitor) -> void;
};

}   // namespace util

template <typename... Ts>
template <typename Visitor>
constexpr auto util::TypeList<Ts...>::for_each(Visitor&& visitor) -> void
{
    [&visitor]<size_t... Is>(std::index_sequence<Is...>) {
        (visitor.template operator()<Ts...[Is]>(), ...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
}

template <typename... Ts>
template <typename IndexedVisitor>
constexpr auto util::TypeList<Ts...>::enumerate(IndexedVisitor&& visitor) -> void
{
    [&visitor]<size_t... Is>(std::index_sequence<Is...>) {
        (visitor.template operator()<Is, Ts...[Is]>(), ...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
}
