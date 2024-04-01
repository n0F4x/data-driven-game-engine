Store::~Store() noexcept
{
    while (!m_map.empty()) {
        m_map.pop_back();
    }
}

template <typename T>
auto Store::emplace(auto&&... t_args) -> T&
{
    return entt::any_cast<T&>(m_map
                                  .try_emplace(
                                      typeid(T),
                                      std::in_place_type<T>,
                                      std::forward<decltype(t_args)>(t_args)...
                                  )
                                  .first.value());
}

template <typename T>
auto Store::find() noexcept -> tl::optional<T&>
{
    auto iter{ m_map.find(typeid(T)) };
    if (iter == m_map.cend()) {
        return tl::nullopt;
    }

    return entt::any_cast<T&>(iter.value());
}

template <typename T>
auto Store::find() const noexcept -> tl::optional<const T&>
{
    const auto iter{ m_map.find(typeid(T)) };
    if (iter == m_map.cend()) {
        return tl::nullopt;
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
