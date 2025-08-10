module;

#include <type_traits>

export module ddge.utility.meta.type_traits.underlying;

template <typename>
struct helper;

template <template <typename> typename Wrapper_T, typename T>
struct helper<Wrapper_T<T>> : std::type_identity<T> {};

namespace ddge::util::meta {

export template <typename T>
struct underlying
    : std::type_identity<typename ::helper<std::remove_reference_t<T>>::type> {};

export template <typename T>
using underlying_t = typename underlying<T>::type;

}   // namespace ddge::util::meta
