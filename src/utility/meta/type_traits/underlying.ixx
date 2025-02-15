export module utility.meta.type_traits.underlying;

namespace util::meta {

export template <typename>
struct underlying;

template <template <typename> typename Wrapper_T, typename T>
struct underlying<Wrapper_T<T>> {
    using type = T;
};

export template <typename T>
using underlying_t = typename underlying<T>::type;

}   // namespace util::meta
