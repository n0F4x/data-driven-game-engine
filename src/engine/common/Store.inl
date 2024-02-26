#include <stdexcept>
#include <string>

#include "Store.hpp"

namespace engine {

Store::~Store() noexcept
{
    while (!m_elements.empty()) {
        m_elements.pop_back();
    }
}

template <typename T>
auto Store::emplace(auto&&... t_args) -> T&
{
    auto [iter, success]{ m_index_map.try_emplace(
        entt::type_index<T>{}, static_cast<TypeIndex>(m_elements.size())
    ) };

    if (!success) {
        return entt::any_cast<T&>(m_elements.at(iter->second));
    }

    return entt::any_cast<T&>(m_elements.emplace_back(
        std::in_place_type<T>, std::forward<decltype(t_args)>(t_args)...
    ));
}

template <typename T>
auto Store::emplace_or_replace(auto&&... t_args) -> T&
{
    auto [iter, success]{ m_index_map.try_emplace(
        entt::type_index<T>{}, static_cast<TypeIndex>(m_elements.size())
    ) };

    if (!success) {
        return entt::any_cast<T&>(m_elements[iter->second]
               ) = T{ std::forward<decltype(t_args)>(t_args)... };
    }

    return entt::any_cast<T&>(m_elements.emplace_back(
        std::in_place_type<T>, std::forward<decltype(t_args)>(t_args)...
    ));
}

template <typename T>
auto Store::find() noexcept -> tl::optional<T&>
{
    const auto index_iter{ m_index_map.find(entt::type_index<T>{}) };
    if (index_iter == m_index_map.cend()) {
        return tl::nullopt;
    }

    return entt::any_cast<T&>(m_elements.at(index_iter->second));
}

template <typename T>
auto Store::find() const noexcept -> tl::optional<const T&>
{
    const auto index_iter{ m_index_map.find(entt::type_index<T>{}) };
    if (index_iter == m_index_map.cend()) {
        return tl::nullopt;
    }

    return entt::any_cast<const T&>(m_elements.at(index_iter->second));
}

template <typename T>
auto Store::at() -> T&
{
    using namespace std::string_literals;

    const auto index_iter{ m_index_map.find(entt::type_index<T>{}) };
    if (index_iter == m_index_map.cend()) {
        throw std::out_of_range{ "Store::at is out of range for type `"s
                                     .append(entt::type_id<T>().name())
                                     .append("`") };
    }

    return entt::any_cast<T&>(m_elements.at(index_iter->second));
}

template <typename T>
auto Store::at() const -> const T&
{
    using namespace std::string_literals;

    const auto index_iter{ m_index_map.find(entt::type_index<T>{}) };
    if (index_iter == m_index_map.cend()) {
        throw std::out_of_range{ "Store::at is out of range for type `"s
                                     .append(entt::type_id<T>().name())
                                     .append("`") };
    }

    return entt::any_cast<const T&>(m_elements.at(index_iter->second));
}

template <typename T>
auto Store::contains() const noexcept -> bool
{
    return m_index_map.contains(entt::type_index<T>{});
}

}   // namespace engine
