#include <type_traits>

import ddge.utility.meta.type_traits.mimic;

static_assert(std::is_same_v<ddge::util::meta::mimic_t<int, float>, int>);
static_assert(std::is_same_v<ddge::util::meta::mimic_t<int, float&>, int&>);
static_assert(std::is_same_v<ddge::util::meta::mimic_t<int, float&&>, int&&>);
static_assert(std::is_same_v<ddge::util::meta::mimic_t<int, const float>, const int>);
static_assert(std::is_same_v<ddge::util::meta::mimic_t<int, const float&>, const int&>);
static_assert(std::is_same_v<ddge::util::meta::mimic_t<int, const float&&>, const int&&>);

static_assert(std::is_same_v<ddge::util::meta::mimic_t<int, volatile float>, volatile int>);
static_assert(std::is_same_v<ddge::util::meta::mimic_t<int, volatile float&>, volatile int&>);
static_assert(std::is_same_v<
              ddge::util::meta::mimic_t<int, volatile float&&>,
              volatile int&&>);
static_assert(std::is_same_v<
              ddge::util::meta::mimic_t<int, const volatile float>,
              const volatile int>);
static_assert(std::is_same_v<
              ddge::util::meta::mimic_t<int, const volatile float&>,
              const volatile int&>);
static_assert(std::is_same_v<
              ddge::util::meta::mimic_t<int, const volatile float&&>,
              const volatile int&&>);
