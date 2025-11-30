module;

#include <concepts>

export module ddge.modules.scheduler.provider_c;

namespace ddge::scheduler {

export template <typename T, typename App_T>
concept provider_c = std::constructible_from<T, App_T&>;

}   // namespace ddge::scheduler
