#include <type_traits>

import utility.meta.type_traits.type_list.type_list_filter;
import utility.TypeList;

static_assert(std::is_same_v<
              util::meta::type_list_filter_t<util::TypeList<float>, std::is_integral>,
              util::TypeList<>>);
static_assert(std::is_same_v<
              util::meta::type_list_filter_t<util::TypeList<int, float>, std::is_integral>,
              util::TypeList<int>>);
static_assert(std::is_same_v<
              util::meta::
                  type_list_filter_t<util::TypeList<int, float, long>, std::is_integral>,
              util::TypeList<int, long>>);
