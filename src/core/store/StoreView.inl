template <typename T>
auto StoreView::find() const noexcept -> std::optional<std::reference_wrapper<T>>
{
    return m_store.get().find<T>();
}

template <typename T>
auto StoreView::at() const -> T&
{
    return m_store.get().at<T>();
}

template <typename T>
auto StoreView::contains() const noexcept -> bool
{
    return m_store.get().contains<T>();
}
