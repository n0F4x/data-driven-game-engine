module;

#include <functional>
#include <memory>
#include <optional>
#include <typeindex>

#include <tsl/ordered_map.h>

#include <entt/core/any.hpp>

export module core.store.Store;

namespace core::store {

export class Store {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Store()                 = default;
    Store(const Store&)     = delete;
    Store(Store&&) noexcept = default;
    ~Store() noexcept;

    auto operator=(const Store&) -> Store&     = delete;
    auto operator=(Store&&) noexcept -> Store& = default;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename T>
    auto emplace(auto&&... args) -> T&;

    template <typename T>
    [[nodiscard]]
    auto find() noexcept -> std::optional<std::reference_wrapper<T>>;
    template <typename T>
    [[nodiscard]]
    auto find() const noexcept -> std::optional<std::reference_wrapper<const T>>;

    template <typename T>
    [[nodiscard]]
    auto at() -> T&;
    template <typename T>
    [[nodiscard]]
    auto at() const -> const T&;

    template <typename T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    tsl::ordered_map<std::type_index, std::unique_ptr<entt::any>> m_map;
};

}

template <typename T>
auto core::store::Store::emplace(auto&&... args) -> T&
{
    return entt::any_cast<T&>(*m_map
                                   .try_emplace(
                                       typeid(T),
                                       std::make_unique<entt::any>(
                                           std::in_place_type<T>,
                                           std::forward<decltype(args)>(args)...
                                       )
                                   )
                                   .first.value());
}

template <typename T>
auto core::store::Store::find() noexcept -> std::optional<std::reference_wrapper<T>>
{
    auto iter{ m_map.find(typeid(T)) };
    if (iter == m_map.cend()) {
        return std::nullopt;
    }

    return entt::any_cast<T&>(*iter.value());
}

template <typename T>
auto core::store::Store::find() const noexcept -> std::optional<std::reference_wrapper<const T>>
{
    const auto iter{ m_map.find(typeid(T)) };
    if (iter == m_map.cend()) {
        return std::nullopt;
    }

    return entt::any_cast<const T&>(*iter.value());
}

template <typename T>
auto core::store::Store::at() -> T&
{
    return entt::any_cast<T&>(*m_map.at(typeid(T)));
}

template <typename T>
auto core::store::Store::at() const -> const T&
{
    return entt::any_cast<const T&>(*m_map.at(typeid(T)));
}

template <typename T>
auto core::store::Store::contains() const noexcept -> bool
{
    return m_map.contains(typeid(T));
}
