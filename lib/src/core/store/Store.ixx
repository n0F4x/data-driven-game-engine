module;

#include <functional>
#include <memory>
#include <optional>
#include <typeindex>

#include <tsl/ordered_map.h>

export module core.store.Store;

import utility.containers.Any;

export import core.store.storable_c;

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
    template <storable_c T, typename... Args_T>
    auto emplace(Args_T&&... args) -> T&;

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
    tsl::ordered_map<std::type_index, util::BasicAny<0>> m_map;
};

}   // namespace core::store

template <core::store::storable_c T, typename... Args_T>
auto core::store::Store::emplace(Args_T&&... args) -> T&
{
    return util::any_cast<T>(
        m_map
            .try_emplace(
                typeid(T), std::in_place_type<T>, std::forward<Args_T>(args)...

            )
            .first.value()
    );
}

template <typename T>
auto core::store::Store::find() noexcept -> std::optional<std::reference_wrapper<T>>
{
    auto iter{ m_map.find(typeid(T)) };
    if (iter == m_map.cend()) {
        return std::nullopt;
    }

    return util::any_cast<T>(iter.value());
}

template <typename T>
auto core::store::Store::find() const noexcept
    -> std::optional<std::reference_wrapper<const T>>
{
    const auto iter{ m_map.find(typeid(T)) };
    if (iter == m_map.cend()) {
        return std::nullopt;
    }

    return util::any_cast<T>(iter.value());
}

template <typename T>
auto core::store::Store::at() -> T&
{
    return util::any_cast<T>(m_map.at(typeid(T)));
}

template <typename T>
auto core::store::Store::at() const -> const T&
{
    return util::any_cast<T>(m_map.at(typeid(T)));
}

template <typename T>
auto core::store::Store::contains() const noexcept -> bool
{
    return m_map.contains(typeid(T));
}
