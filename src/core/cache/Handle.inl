namespace core::cache {

template <typename Resource>
auto make_handle(auto&&... t_args) -> cache::Handle<Resource>
{
    return std::make_shared<Resource>(std::forward<decltype(t_args)>(t_args)...);
}

template <typename Resource, tuple_like Tuple>
auto make_handle(Tuple&& tuple) -> Handle<Resource>
{
    return std::apply(
        []<typename... Args>(Args... args) {
            return make_handle<Resource>(std::forward<Args>(args)...);
        },
        std::forward<Tuple>(tuple)
    );
}

}   // namespace core::cache
