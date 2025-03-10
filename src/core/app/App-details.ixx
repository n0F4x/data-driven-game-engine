module;

#include <algorithm>
#include <tuple>

export module core.app.App:details;

import utility.meta.type_traits.integer_sequence.integer_sequence_offset;
import utility.meta.type_traits.type_list.type_list_drop_front;

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

template <typename... Addons_T>
using old_app_t = util::meta::type_list_drop_front_t<core::app::App<Addons_T...>>;
