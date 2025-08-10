module;

#include <concepts>

export module ddge.modules.execution.provider_c;

namespace ddge::exec {

export template <typename T, typename App_T>
concept provider_c = std::constructible_from<T, App_T&>;

}   // namespace ddge::exec
