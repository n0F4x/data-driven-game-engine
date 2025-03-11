module;

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

#ifdef ENGINE_ENABLE_STATIC_TESTS
  #include <cassert>
#endif

export module core.resource.ResourceManager;

import utility.containers.StackedTuple;
import utility.meta.type_traits.all_different;
import utility.meta.concepts.decayed;
import utility.meta.type_traits.functional.invoke_result_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

template <typename T, typename Resource_T>
concept decays_to_factory_c = std::constructible_from<
    Resource_T,
    util::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<T>>>>;

namespace core::resource {

export template <typename T>
concept resource_c = ::util::meta::decayed_c<T>;

export template <resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
class ResourceManager {
public:
    template <::decays_to_factory_c<Resources_T>... Factories_T>
    constexpr explicit ResourceManager(Factories_T&&... factories);

    template <typename Resource_T, typename Self_T>
        requires(::util::meta::
                     type_list_contains_v<::util::TypeList<Resources_T...>, Resource_T>)
    [[nodiscard]]
    constexpr auto get(this Self_T&&) -> std::
        conditional_t<std::is_const_v<Self_T>, std::add_const_t<Resource_T&>, Resource_T&>;

private:
    std::unique_ptr<::util::StackedTuple<Resources_T...>> m_resources;
};

}   // namespace core::resource

template <core::resource::resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
template <decays_to_factory_c<Resources_T>... Factories_T>
constexpr core::resource::ResourceManager<Resources_T...>::ResourceManager(
    Factories_T&&... factories
)
    : m_resources{ std::make_unique<util::StackedTuple<Resources_T...>>(
          std::forward<Factories_T>(factories)...
      ) }
{}

template <core::resource::resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
template <typename Resource_T, typename Self_T>
    requires(::util::meta::
                 type_list_contains_v<::util::TypeList<Resources_T...>, Resource_T>)
constexpr auto core::resource::ResourceManager<Resources_T...>::get(this Self_T&& self)
    -> std::
        conditional_t<std::is_const_v<Self_T>, std::add_const_t<Resource_T&>, Resource_T&>
{
    return self.m_resources->template get<Resource_T>();
}

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

// TODO:
// remove unnamed namespace when clang permits using the same definition in different
// module fragments
namespace {

struct First {
    int value{ 42 };
};

struct Second {
    std::reference_wrapper<const int> ref;
};

[[nodiscard]]
constexpr auto make_first() -> First
{
    return First{};
}

[[nodiscard]]
constexpr auto make_second(const First& first) -> Second
{
    return Second{ .ref = first.value };
}

}   // namespace

static_assert(
    [] {
        core::resource::ResourceManager<First, Second> resource_manager{ make_first,
                                                                         make_second };
        auto moved_resource_manager{ std::move(resource_manager) };

        assert(
            moved_resource_manager.get<First>().value
            == moved_resource_manager.get<Second>().ref.get()
        );

        return true;
    }(),
    "move test failed"
);

#endif
