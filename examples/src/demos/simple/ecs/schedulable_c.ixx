module;

#include <type_traits>

export module ecs:schedulable_c;

import :Query;
import :Res;

import utility.concepts.functional.callable_c;
import utility.concepts.type_list.all_of_type_list_c;
import utility.type_traits.functional.arguments_of;
import utility.type_traits.is_specialization_of;

namespace ecs {

template <typename T>
struct is_specialization_of_Res : util::meta::is_specialization_of<T, Res> {};

template <typename T>
struct is_specialization_of_Query : util::meta::is_specialization_of<T, Query> {};

template <typename T>
struct is_valid_argument
    : std::disjunction<is_specialization_of_Res<T>, is_specialization_of_Query<T>> {};

export template <typename SchedulableT>
concept schedulable_c =
    util::meta::callable_c<std::remove_pointer_t<std::decay_t<SchedulableT>>>
    && util::meta::all_of_type_list_c<
        util::meta::arguments_of_t<std::remove_pointer_t<std::decay_t<SchedulableT>>>,
        is_valid_argument>;

}   // namespace ecs
