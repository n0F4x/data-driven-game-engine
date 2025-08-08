module;

#include <concepts>
#include <type_traits>

export module core.scheduler.raw_task_c;

import utility.meta.concepts.functional.unambiguously_invocable;
import utility.meta.concepts.type_list.type_list_all_of;
import utility.meta.type_traits.functional.arguments_of;

// TODO: remove anonimous namespace with better Clang
namespace {

template <typename T>
struct IsConstructibleFromReference
    : std::bool_constant<std::constructible_from<T, std::remove_cvref_t<T>&>> {};

}   // namespace

namespace core::scheduler {

export template <typename T>
concept raw_task_c =
    util::meta::unambiguously_invocable_c<T>
    && util::meta::
        type_list_all_of_c<util::meta::arguments_of_t<T>, ::IsConstructibleFromReference>;

}   // namespace core::scheduler
