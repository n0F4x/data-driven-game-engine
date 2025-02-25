module;

#include <array>
#include <concepts>
#include <memory>
#include <memory_resource>
#include <ranges>
#include <type_traits>
#include <utility>

export module core.resource.ResourceManager;

import utility.memory.Deallocator;
import utility.meta.type_traits.all_different;
import utility.meta.concepts.decayed;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.functional.invoke_result_of;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.tuple.tuple_drop_front;
import utility.TypeList;

template <typename T, typename Resource_T>
concept decays_to_factory_c = std::constructible_from<
    Resource_T,
    util::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<T>>>>;

namespace core::resource {

export template <typename T>
concept resource_c = ::util::meta::decayed_c<T>;

// TODO:
// create new tuple type that can be used in ResourceManager, so it can become
// constexpr without std::bit_cast

export template <resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
class ResourceManager {
public:
    template <::decays_to_factory_c<Resources_T>... Factories_T>
    explicit ResourceManager(Factories_T&&... factories);
    ResourceManager(ResourceManager&&) = default;
    ~ResourceManager();

    auto operator=(ResourceManager&&) -> ResourceManager& = default;

    template <typename Resource_T, typename Self_T>
        requires(::util::meta::
                     type_list_contains_v<::util::TypeList<Resources_T...>, Resource_T>)
    [[nodiscard]]
    auto get(this Self_T&&) -> std::
        conditional_t<std::is_const_v<Self_T>, std::add_const_t<Resource_T&>, Resource_T&>;

private:
    using ResourcesTuple = std::tuple<Resources_T...>;
    using Buffer         = std::array<std::byte, sizeof(ResourcesTuple)>;
    template <typename Resource_T>
    using resource_handle_t = std::reference_wrapper<Resource_T>;

    std::unique_ptr<Buffer> m_buffer{ std::make_unique<Buffer>() };

    static auto cast_to_resources_tuple(Buffer& buffer) -> ResourcesTuple&;
    static auto cast_to_resources_tuple(const Buffer& buffer) -> const ResourcesTuple&;

    template <typename... Factories_T>
    static auto construct_resources(Buffer& buffer, Factories_T&&... factories) -> void;

    static auto destroy_resources(Buffer& buffer) -> void;
};

}   // namespace core::resource

template <core::resource::resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
template <decays_to_factory_c<Resources_T>... Factories_T>
core::resource::ResourceManager<Resources_T...>::ResourceManager(
    Factories_T&&... factories
)
{
    construct_resources(*m_buffer, std::forward<Factories_T>(factories)...);
}

template <core::resource::resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
core::resource::ResourceManager<Resources_T...>::~ResourceManager<Resources_T...>()
{
    if (m_buffer != nullptr) {
        destroy_resources(*m_buffer);
    }
}

template <core::resource::resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
template <typename Resource_T, typename Self_T>
    requires(::util::meta::
                 type_list_contains_v<::util::TypeList<Resources_T...>, Resource_T>)
auto core::resource::ResourceManager<Resources_T...>::get(this Self_T&& self) -> std::
    conditional_t<std::is_const_v<Self_T>, std::add_const_t<Resource_T&>, Resource_T&>
{
    return std::get<Resource_T>(cast_to_resources_tuple(*self.m_buffer));
}

template <typename>
struct gather_resources_helper;

template <template <typename...> typename TypeList_T, typename... SelectedTypes_T>
struct gather_resources_helper<TypeList_T<SelectedTypes_T...>> {
    template <typename... Ts>
    [[nodiscard]]
    static auto operator()(std::tuple<std::reference_wrapper<Ts>...>& tuple)
        -> std::tuple<std::reference_wrapper<std::remove_cvref_t<SelectedTypes_T>>...>
    {
        return { std::get<std::reference_wrapper<std::remove_cvref_t<SelectedTypes_T>>>(
            tuple
        )... };
    }
};

template <typename Callable_T, typename... Ts>
auto gather_resources(std::tuple<std::reference_wrapper<Ts>...>& resources_tuple)
{
    using RequiredResourcesTuple_T = util::meta::arguments_of_t<Callable_T>;

    return gather_resources_helper<RequiredResourcesTuple_T>::operator()(resources_tuple);
}

template <core::resource::resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
auto core::resource::ResourceManager<Resources_T...>::cast_to_resources_tuple(
    Buffer& buffer
) -> ResourcesTuple&
{
    return *std::bit_cast<ResourcesTuple*>(buffer.data());
}

template <core::resource::resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
auto core::resource::ResourceManager<Resources_T...>::cast_to_resources_tuple(
    const Buffer& buffer
) -> const ResourcesTuple&
{
    return *std::bit_cast<const ResourcesTuple*>(buffer.data());
}

template <core::resource::resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
template <typename... Factories_T>
auto core::resource::ResourceManager<Resources_T...>::construct_resources(
    Buffer& buffer,
    Factories_T&&... factories
) -> void
{
    [&buffer]<typename... XResources_T, typename... XFactories_T>(
        this auto                                      self,
        std::tuple<resource_handle_t<XResources_T>...> resources_tuple,
        std::tuple<XFactories_T...>&&                  factories_tuple
    ) {
        if constexpr (sizeof...(XFactories_T) == 0) {
            return resources_tuple;
        }
        else {
            using Resource = Resources_T...
                [std::tuple_size_v<std::tuple<resource_handle_t<XResources_T>...>>];

            return self(
                std::tuple_cat(
                    std::move(resources_tuple),
                    std::tuple{ std::ref(*std::construct_at(
                        std::addressof(std::get<Resource>(cast_to_resources_tuple(buffer))),
                        std::apply(
                            std::move(std::get<0>(factories_tuple)),
                            gather_resources<std::remove_pointer_t<
                                std::decay_t<XFactories_T...[0]>>>(resources_tuple)
                        )
                    )) }
                ),
                util::tuple_drop_front(std::move(factories_tuple))
            );
        }
    }(std::tuple<>{}, std::forward_as_tuple(std::forward<Factories_T>(factories)...));
}

template <core::resource::resource_c... Resources_T>
    requires(util::meta::all_different_v<Resources_T...>)
auto core::resource::ResourceManager<Resources_T...>::destroy_resources(Buffer& buffer)
    -> void
{
    ResourcesTuple& resources_tuple{ cast_to_resources_tuple(buffer) };

    [&resources_tuple]<size_t... Is>(std::index_sequence<Is...>) {
        (std::destroy_at(
             std::addressof(std::get<sizeof...(Resources_T) - Is - 1>(resources_tuple))
         ),
         ...);
    }(std::make_index_sequence<sizeof...(Resources_T)>{});
}
