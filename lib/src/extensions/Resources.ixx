module;

#include <algorithm>
#include <any>
#include <functional>
#include <type_traits>
#include <utility>

export module extensions.Resources;

import core.app.Builder;
import core.app.decays_to_app_c;
import core.app.decays_to_builder_c;
import core.app.has_addons_c;
import core.store.Store;

import utility.meta.algorithms.apply;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.meta.type_traits.type_list.type_list_drop_back;
import utility.meta.type_traits.back;
import utility.tuple;

import addons.Resources;

import core.resources.resource_c;

namespace extensions {

export template <typename T>
concept resource_c = core::resource::resource_c<T>;

export template <typename T>
concept injection_c = resource_c<util::meta::result_of_t<T>>;

export template <typename T>
concept decays_to_injection_c = injection_c<std::decay_t<T>>;

export struct ResourcesTag {};

template <extensions::injection_c... Injections_T>
class BasicResources;

template <typename... Injections_T>
using old_resources_t =
    util::meta::type_list_drop_back_t<extensions::BasicResources<Injections_T...>>;

template <extensions::injection_c... Injections_T>
class BasicResources : public ResourcesTag {
public:
    BasicResources() = default;

    template <typename OldBasicResources_T, typename... Args_T>
        requires std::same_as<
            std::remove_cvref_t<OldBasicResources_T>,
            old_resources_t<Injections_T...>>
    constexpr BasicResources(
        OldBasicResources_T&& old_resources,
        std::in_place_t,
        Args_T&&... args
    );

    template <core::app::decays_to_builder_c Self_T, typename Resource_T>
        requires extensions::resource_c<std::remove_cvref_t<Resource_T>>
    constexpr auto use_resource(this Self_T&&, Resource_T&& resource);

    template <
        core::app::decays_to_builder_c    Self_T,
        extensions::decays_to_injection_c Injection_T>
    constexpr auto inject_resource(this Self_T&&, Injection_T&& injection);

    template <core::app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) &&;

private:
    template <extensions::injection_c...>
    friend class BasicResources;

    std::tuple<std::decay_t<Injections_T>...> m_injections;
};

export using Resources = BasicResources<>;

export inline constexpr Resources resources;

}   // namespace extensions

template <extensions::injection_c... Injections_T>
template <typename OldBasicResources_T, typename... Args>
    requires std::same_as<
        std::remove_cvref_t<OldBasicResources_T>,
        extensions::old_resources_t<Injections_T...>>
constexpr extensions::BasicResources<Injections_T...>::BasicResources(
    OldBasicResources_T&& old_resources,
    std::in_place_t,
    Args&&... args
)
    : m_injections{ std::tuple_cat(
          std::forward_like<OldBasicResources_T>(old_resources.m_injections),
          std::make_tuple(
              std::decay_t<util::meta::back_t<Injections_T...>>(std::forward<Args>(args
              )...)
          )
      ) }
{}

template <extensions::injection_c... Injections_T>
template <core::app::decays_to_builder_c Self_T, typename Resource_T>
    requires extensions::resource_c<std::remove_cvref_t<Resource_T>>
constexpr auto extensions::BasicResources<Injections_T...>::use_resource(
    this Self_T&& self,
    Resource_T&&  resource
)
{
    using Resource = std::remove_cvref_t<Resource_T>;

    struct Injection {
        constexpr auto operator()() && -> Resource
        {
            return std::move(resource);
        }

        Resource resource;
    };

    return std::forward<Self_T>(self).inject_resource(
        Injection{ std::forward<Resource_T>(resource) }
    );
}

template <typename>
struct gather_helper;

template <template <typename...> typename TypeList_T, typename... SelectedTypes_T>
struct gather_helper<TypeList_T<SelectedTypes_T...>> {
    template <typename... Ts>
    [[nodiscard]]
    constexpr static auto operator()(std::tuple<Ts...>& tuple)
        -> std::tuple<SelectedTypes_T...>
    {
        return { std::forward_like<SelectedTypes_T>(
            std::get<std::decay_t<SelectedTypes_T>>(tuple)
        )... };
    }
};

template <typename Callable_T, typename... Ts>
constexpr auto gather_parameters(std::tuple<Ts...>& tuple)
{
    using RequiredResourcesTuple_T = util::meta::arguments_of_t<Callable_T>;

    return gather_helper<RequiredResourcesTuple_T>::operator()(tuple);
}

template <extensions::injection_c... Injections_T>
template <core::app::decays_to_builder_c Self_T, extensions::decays_to_injection_c Injection_T>
constexpr auto extensions::BasicResources<Injections_T...>::inject_resource(
    this Self_T&& self,
    Injection_T&& injection
)
{
    if constexpr (requires { requires std::is_function_v<decltype(Injection_T::setup)>; })
    {
        std::apply(
            Injection_T::setup,
            ::gather_parameters<decltype(Injection_T::setup)>(self.m_injections)
        );
    }
    else if constexpr (requires {
                           requires std::
                               is_member_function_pointer_v<decltype(&Injection_T::setup)>;
                       })
    {
        std::apply(
            std::bind_front(&Injection_T::setup, injection),
            ::gather_parameters<decltype(&Injection_T::setup)>(self.m_injections)
        );
    }

    return core::app::swap_extension<BasicResources>(
        std::forward<Self_T>(self),
        [&]<typename BasicResources_T>
            requires(std::is_same_v<std::remove_cvref_t<BasicResources_T>, BasicResources>)
        (BasicResources_T&& resources) {
            return BasicResources<Injections_T..., std::decay_t<Injection_T>>{
                std::forward<BasicResources_T>(resources),
                std::in_place,
                std::forward<Injection_T>(injection)
            };
        }
        );
}

template <extensions::injection_c... Injections_T>
template <core::app::decays_to_app_c App_T>
constexpr auto extensions::BasicResources<Injections_T...>::build(App_T&& app) &&
{
    using ResourcesAddon = addons::Resources<util::meta::result_of_t<Injections_T>...>;

    static_assert(!core::app::has_addons_c<App_T, ResourcesAddon>);

    return util::meta::apply<std::make_index_sequence<sizeof...(Injections_T)>>(
        [this, &app]<size_t... Is> {
            return std::forward<App_T>(app).add_on(
                ResourcesAddon{ std::in_place, std::move(std::get<Is>(m_injections))... }
            );
        }
    );
}
