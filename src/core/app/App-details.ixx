module;

#include <algorithm>
#include <tuple>

export module core.app.App:details;

import utility.meta.offset;

import :mixin_c;
import :fwd;

template <typename Mixin_T, typename... RestOfMixins_T>
class AppBase : public AppBase<RestOfMixins_T...>, public Mixin_T {
protected:
    constexpr AppBase() = default;

    template <typename Base_T, typename... Args_T>
    constexpr explicit AppBase(Base_T&& base, Args_T&&... args)
        : AppBase<RestOfMixins_T...>{ std::forward<Base_T>(base) },
          Mixin_T(std::forward<Args_T>(args)...)
    {}
};

class MonoMixin {};

template <>
class AppBase<MonoMixin> {};

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

template <typename... Mixins_T>
using old_app_t = std::conditional_t<
    sizeof...(Mixins_T) != 0,
    typename old_app<
        utils::meta::offset_t<
            std::make_index_sequence<std::max(static_cast<int>(sizeof...(Mixins_T)) - 1, 0)>,
            1>,
        Mixins_T...>::type,
    void>;
