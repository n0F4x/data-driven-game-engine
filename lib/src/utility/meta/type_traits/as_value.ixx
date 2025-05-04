module;

#include <functional>
#include <type_traits>

export module utility.meta.type_traits.as_value;

namespace util::meta {

template <typename T>
struct as_value : std::type_identity<T> {};

template <typename T>
    requires(std::is_lvalue_reference_v<T>)
struct as_value<T>
    : std::type_identity<std::reference_wrapper<std::remove_reference_t<T>>> {};

template <typename T>
    requires(std::is_rvalue_reference_v<T>)
struct as_value<T>;

export template <typename T>
using as_value_t = typename as_value<T>::type;

}   // namespace util::meta
