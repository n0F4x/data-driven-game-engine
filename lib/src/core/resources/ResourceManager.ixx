module;

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

export module core.resources.ResourceManager;

import utility.containers.StackedTuple;
import utility.meta.concepts.all_different;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.functional.result_of;
import utility.TypeList;

import core.resources.resource_c;

namespace core::resources {

export template <resource_c... Resources_T>
    requires util::meta::all_different_c<Resources_T...>
class ResourceManager {
public:
    template <resource_c Resource_T>
    constexpr static std::bool_constant<
        util::meta::type_list_contains_v<util::TypeList<Resources_T...>, Resource_T>>
        contains;

    template <typename... Factories_T>
        requires std::
            constructible_from<util::StackedTuple<Resources_T...>, Factories_T&&...>
        constexpr explicit ResourceManager(Factories_T&&... factories);

    template <typename Resource_T, typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&) -> std::
        conditional_t<std::is_const_v<Self_T>, std::add_const_t<Resource_T&>, Resource_T&>
        requires(contains<Resource_T>());

private:
    std::unique_ptr<::util::StackedTuple<Resources_T...>> m_resources;
};

template <typename... Factories_T>
    requires std::constructible_from<
        util::StackedTuple<util::meta::result_of_t<Factories_T>...>,
        Factories_T&&...>
ResourceManager(Factories_T&&...)
    -> ResourceManager<util::meta::result_of_t<Factories_T>...>;

}   // namespace core::resources

template <core::resources::resource_c... Resources_T>
    requires util::meta::all_different_c<Resources_T...>
template <typename... Factories_T>
    requires std::constructible_from<util::StackedTuple<Resources_T...>, Factories_T&&...>
constexpr core::resources::ResourceManager<Resources_T...>::ResourceManager(
    Factories_T&&... factories
)
    : m_resources{ std::make_unique<util::StackedTuple<Resources_T...>>(
          std::forward<Factories_T>(factories)...
      ) }
{}

template <core::resources::resource_c... Resources_T>
    requires util::meta::all_different_c<Resources_T...>
template <typename Resource_T, typename Self_T>
constexpr auto core::resources::ResourceManager<Resources_T...>::get(this Self_T&& self)
    -> std::
        conditional_t<std::is_const_v<Self_T>, std::add_const_t<Resource_T&>, Resource_T&>
    requires(contains<Resource_T>())
{
    return self.m_resources->template get<Resource_T>();
}
