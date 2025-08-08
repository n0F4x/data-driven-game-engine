module;

#include <concepts>

export module core.scheduler.provider_c;

namespace core::scheduler {

export template <typename T, typename App_T>
concept provider_c = std::constructible_from<T, App_T&>;

}   // namespace core::scheduler
