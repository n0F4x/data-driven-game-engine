module;

#include <functional>
#include <type_traits>

export module ddge.utility.wrap_ref;

namespace ddge::util {

export template <typename T>
[[nodiscard]]
constexpr auto wrap_ref(T&&) -> std::conditional_t<
    std::is_lvalue_reference_v<T>,
    std::reference_wrapper<std::remove_reference_t<T>>,
    T>;

}   // namespace ddge::util

template <typename T>
constexpr auto ddge::util::wrap_ref(T&& arg) -> std::conditional_t<
    std::is_lvalue_reference_v<T>,
    std::reference_wrapper<std::remove_reference_t<T>>,
    T>
{
    if constexpr (std::is_lvalue_reference_v<T>) {
        return std::ref(arg);
    }
    else {
        return arg;
    }
}
