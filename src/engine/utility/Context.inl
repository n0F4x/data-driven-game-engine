namespace engine::utils {

Context::~Context() noexcept
{
    while (!m_elements.empty()) {
        m_elements.pop_back();
    }
}

template <typename T>
auto Context::emplace(auto&&... t_args) noexcept
    -> std::add_lvalue_reference_t<std::remove_cv_t<T>>
{
    using Type = std::remove_cvref_t<T>;

    m_index_map.try_emplace(entt::type_index<Type>{}, m_elements.size());

    return entt::any_cast<Type&>(m_elements.emplace_back(any{
        std::in_place_type<Type>, std::forward<decltype(t_args)>(t_args)... }));
}

template <typename T>
auto Context::find() noexcept -> opview::optional_view<std::remove_cv_t<T>>
{
    using Type = std::remove_cvref_t<T>;

    auto index_iter{ m_index_map.find(entt::type_index<Type>{}) };
    if (index_iter == m_index_map.end()) {
        return std::nullopt;
    }

    return entt::any_cast<Type&>(m_elements.at(index_iter->second));
}

template <typename T>
auto Context::contains() const noexcept -> bool
{
    using Type = std::remove_cvref_t<T>;

    return m_index_map.contains(entt::type_index<Type>{});
}

}   // namespace engine::utils
