module;

#include <algorithm>
#include <tuple>
#include <utility>

export module core.app.Builder:details;

import :fwd;

import utility.type_traits.integer_sequence.offset_integer_sequence;

import core.app.App;

template <typename Extension_T, typename... RestOfExtensions_T>
class BuilderBase : public BuilderBase<RestOfExtensions_T...>, public Extension_T {
public:
    constexpr BuilderBase() = default;

    template <typename Base_T, typename... Args_T>
    constexpr explicit BuilderBase(Base_T&& base, std::in_place_t, Args_T&&... args)
        : BuilderBase<RestOfExtensions_T...>{ std::forward<Base_T>(base) },
          Extension_T(std::forward<Args_T>(args)...)
    {}

    template <typename Self_T, typename App_T>
        requires core::app::app_c<App_T>
    constexpr auto build(this Self_T&& self, App_T&& app)
    {
        if constexpr (class DummyAddon{}; requires(core::app::App<DummyAddon> dummy_app) {
                          core::app::app_c<
                              decltype(std::forward<Self_T>(self).Extension_T::operator()(
                                  std::forward_like<Self_T>(dummy_app)
                              ))>;
                      })
        {
            return std::forward<Self_T>(self).Extension_T::operator()(
                std::forward<Self_T>(self).BuilderBase<RestOfExtensions_T...>::build(
                    std::forward<App_T>(app)
                )
            );
        }
        else {
            return std::forward<Self_T>(self).BuilderBase<RestOfExtensions_T...>::build(
                std::forward<App_T>(app)
            );
        }
    }
};

class RootExtension {};

template <>
class BuilderBase<RootExtension> {
public:
    template <typename App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) -> App_T
    {
        return std::forward<App_T>(app);
    }
};

template <typename T, typename... Ts>
struct old_builder;

template <size_t... I, typename... Ts>
struct old_builder<std::index_sequence<I...>, Ts...> {
    using type = core::app::Builder<std::tuple_element_t<I, std::tuple<Ts...>>...>;
};

template <typename T>
struct old_builder<std::integer_sequence<size_t>, T> {
    using type = core::app::Builder<>;
};

template <typename... Extensions_T>
using old_builder_t = std::conditional_t<
    sizeof...(Extensions_T) != 0,
    typename old_builder<
        util::meta::offset_integer_sequence_t<
            std::make_index_sequence<
                std::max(static_cast<int>(sizeof...(Extensions_T)) - 1, 0)>,
            1>,
        Extensions_T...>::type,
    void>;
