module;

#include <algorithm>
#include <tuple>

export module core.app.App:details;

import utility.meta.type_traits.integer_sequence.offset_integer_sequence;

import :addon_c;
import :fwd;

template <typename Addon_T, typename... RestOfAddons_T>
class AppBase : public AppBase<RestOfAddons_T...>, public Addon_T {
protected:
    constexpr AppBase() = default;

    template <typename Base_T, typename... Args_T>
    constexpr explicit AppBase(Base_T&& base, std::in_place_t, Args_T&&... args)
        : AppBase<RestOfAddons_T...>{ std::forward<Base_T>(base) },
          Addon_T(std::forward<Args_T>(args)...)
    {}
};

class RootAddon {};

template <>
class AppBase<RootAddon> {};

template <typename T, typename... Ts>
struct old_app;

template <size_t... I, typename... Ts>
struct old_app<std::index_sequence<I...>, Ts...> {
    using type = core::app::App<std::tuple_element_t<I, std::tuple<Ts...>>...>;
};

template <typename T>
struct old_app<std::integer_sequence<size_t>, T> {
    using type = core::app::App<>;
};

template <typename... Addons_T>
using old_app_t = std::conditional_t<
    sizeof...(Addons_T) != 0,
    typename old_app<
        util::meta::offset_integer_sequence_t<
            std::make_index_sequence<std::max(static_cast<int>(sizeof...(Addons_T)) - 1, 0)>,
            1>,
        Addons_T...>::type,
    void>;
