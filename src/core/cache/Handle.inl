namespace core::cache {

template <typename Resource>
auto make_handle(auto&&... t_args) -> cache::Handle<Resource>
{
    return std::make_shared<Resource>(std::forward<decltype(t_args)>(t_args)...);
}

}   // namespace core
