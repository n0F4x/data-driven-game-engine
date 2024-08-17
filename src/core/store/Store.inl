template <typename T>
auto Store::emplace(auto&&... args) -> T&
{
    return entt::any_cast<T&>(
        m_map
            .try_emplace(
                typeid(T), std::in_place_type<T>, std::forward<decltype(args)>(args)...
            )
            .first.value()
    );
}

template <typename T>
auto Store::find() noexcept -> std::optional<std::reference_wrapper<T>>
{
    auto iter{ m_map.find(typeid(T)) };
    if (iter == m_map.cend()) {
        return std::nullopt;
    }

    return entt::any_cast<T&>(iter.value());
}

template <typename T>
auto Store::find() const noexcept -> std::optional<std::reference_wrapper<const T>>
{
    const auto iter{ m_map.find(typeid(T)) };
    if (iter == m_map.cend()) {
        return std::nullopt;
    }

    return entt::any_cast<const T&>(iter.value());
}

template <typename T>
auto Store::at() -> T&
{
    return entt::any_cast<T&>(m_map.at(typeid(T)));
}

template <typename T>
auto Store::at() const -> const T&
{
    return entt::any_cast<const T&>(m_map.at(typeid(T)));
}

template <typename T>
auto Store::contains() const noexcept -> bool
{
    return m_map.contains(typeid(T));
}
