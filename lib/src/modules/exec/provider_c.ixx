module;

#include <concepts>

export module ddge.modules.exec.provider_c;

namespace ddge::exec {

export template <typename T, typename App_T>
concept provider_c = std::constructible_from<T, App_T&>;

}   // namespace ddge::exec
