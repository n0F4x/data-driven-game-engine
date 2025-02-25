module;

#include <algorithm>
#include <any>
#include <functional>
#include <type_traits>
#include <utility>

export module extensions.ResourceManager;

import core.app.App;
import core.app.Builder;
import core.store.Store;

import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.invoke_result_of;
import utility.meta.type_traits.type_list.type_list_drop_back;
import utility.meta.type_traits.select_last;
import utility.tuple;

import addons.ResourceManager;

namespace extensions {

export template <typename Resource_T>
concept resource_c = addons::resource_c<Resource_T>;

export template <typename Injection_T>
concept injection_c = resource_c<
    util::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<Injection_T>>>>;

}   // namespace extensions

template <extensions::injection_c... Injections_T>
class ResourceManager;

namespace extensions {

export using ResourceManager = ::ResourceManager<>;

export struct ResourceManagerTag {};

}   // namespace extensions

template <typename... Injections_T>
using old_resource_manager_t =
    util::meta::type_list_drop_back_t<ResourceManager<Injections_T...>>;

template <extensions::injection_c... Injections_T>
class ResourceManager {
public:
    using ExtensionTag = extensions::ResourceManagerTag;

    constexpr ResourceManager() = default;

    template <typename OldResourceManager, typename... Args>
        requires std::same_as<
            std::remove_cvref_t<OldResourceManager>,
            old_resource_manager_t<Injections_T...>>
    constexpr ResourceManager(
        OldResourceManager&& old_resource_manager,
        std::in_place_t,
        Args&&... args
    );

    template <core::app::builder_c Self_T, typename Resource_T>
        requires extensions::resource_c<std::remove_cvref_t<Resource_T>>
    auto use_resource(this Self_T&&, Resource_T&& resource);

    template <core::app::builder_c Self_T, extensions::injection_c Injection_T>
    auto inject_resource(this Self_T&&, Injection_T&& injection);

protected:
    template <core::app::app_c App_T>
    auto operator()(App_T&& app) &&;

private:
    template <extensions::injection_c...>
    friend class ResourceManager;

    std::tuple<std::decay_t<Injections_T>...> m_injections;
};

template <extensions::injection_c... Injections_T>
template <typename OldResourceManager, typename... Args>
    requires std::same_as<
        std::remove_cvref_t<OldResourceManager>,
        old_resource_manager_t<Injections_T...>>
constexpr ResourceManager<Injections_T...>::ResourceManager(
    OldResourceManager&& old_resource_manager,
    std::in_place_t,
    Args&&... args
)
    : m_injections{ std::tuple_cat(
          std::forward_like<OldResourceManager>(old_resource_manager.m_injections),
          std::make_tuple(
              std::decay_t<util::meta::select_last_t<Injections_T...>>(
                  std::forward<Args>(args)...
              )
          )
      ) }
{}

template <extensions::injection_c... Injections_T>
template <core::app::builder_c Self_T, typename Resource_T>
    requires extensions::resource_c<std::remove_cvref_t<Resource_T>>
auto ResourceManager<Injections_T...>::use_resource(
    this Self_T&& self,
    Resource_T&&  resource
)
{
    using Resource = std::remove_cvref_t<Resource_T>;

    struct Injection {
        auto operator()() -> Resource
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
    static auto operator()(std::tuple<Ts...>& tuple) -> std::tuple<SelectedTypes_T...>
    {
        return { std::get<std::remove_cvref_t<SelectedTypes_T>>(tuple)... };
    }
};

template <typename Callable_T, typename... Ts>
auto gather_parameters(std::tuple<Ts...>& tuple)
{
    using RequiredResourcesTuple_T = util::meta::arguments_of_t<Callable_T>;

    return gather_helper<RequiredResourcesTuple_T>::operator()(tuple);
}

template <extensions::injection_c... Injections_T>
template <core::app::builder_c Self_T, extensions::injection_c Injection_T>
auto ResourceManager<Injections_T...>::inject_resource(
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

    return std::forward<Self_T>(self).template swap_extension<ResourceManager>(
        [&]<typename ResourceManager_T>
            requires(std::is_same_v<std::remove_cvref_t<ResourceManager_T>, ResourceManager>)
        (ResourceManager_T&& resource_manager) {
            return ResourceManager<Injections_T..., std::decay_t<Injection_T>>{
                std::forward<ResourceManager_T>(resource_manager),
                std::in_place,
                std::forward<Injection_T>(injection)
            };
        }
    );
}

template <extensions::injection_c... Injections_T>
template <core::app::app_c App_T>
auto ResourceManager<Injections_T...>::operator()(App_T&& app) &&
{
    using ResourceManagerAddon = addons::ResourceManager<util::meta::invoke_result_of_t<
        std::remove_pointer_t<std::decay_t<Injections_T>>>...>;

    static_assert(!core::app::has_addons_c<App_T, ResourceManagerAddon>);

    return [this, &app]<size_t... Is>(std::index_sequence<Is...>) {
        return std::forward<App_T>(app)
            .template add_on<ResourceManagerAddon>(
                std::in_place, std::move(std::get<Is>(m_injections))...
            )
            .template add_on<addons::ResourceManagerTag>();
    }(std::make_index_sequence<sizeof...(Injections_T)>{});
}
