module;

#include <utility>

export module utility.TypeList;

namespace util {

export template <typename...>
struct TypeList {
    template <typename Visitor>
    static auto for_each(Visitor&& visitor) -> void;
};

}   // namespace util

template <typename... Ts>
template <typename Visitor>
auto util::TypeList<Ts...>::for_each(Visitor&& visitor) -> void
{
    [&visitor]<size_t... Is>(std::index_sequence<Is...>) {
        (visitor.template operator()<Ts...[Is]>(), ...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
}
