#include <type_traits>

import ddge.utility.meta.type_traits.mimic;

namespace {

template <typename T>
consteval auto check_type() -> bool
{
    using Naked = std::remove_cvref_t<T>;

    static_assert(std::is_same_v<ddge::util::meta::mimic_t<T, float>, Naked>);
    static_assert(std::is_same_v<ddge::util::meta::mimic_t<T, float&>, Naked&>);
    static_assert(std::is_same_v<ddge::util::meta::mimic_t<T, float&&>, Naked&&>);
    static_assert(std::is_same_v<ddge::util::meta::mimic_t<T, const float>, const Naked>);
    static_assert(std::is_same_v<ddge::util::meta::mimic_t<T, const float&>, const Naked&>);
    static_assert(std::is_same_v<ddge::util::meta::mimic_t<T, const float&&>, const Naked&&>);

    static_assert(std::is_same_v<
                  ddge::util::meta::mimic_t<T, volatile float>,
                  volatile Naked>);
    static_assert(std::is_same_v<
                  ddge::util::meta::mimic_t<T, volatile float&>,
                  volatile Naked&>);
    static_assert(std::is_same_v<
                  ddge::util::meta::mimic_t<T, volatile float&&>,
                  volatile Naked&&>);
    static_assert(std::is_same_v<
                  ddge::util::meta::mimic_t<T, const volatile float>,
                  const volatile Naked>);
    static_assert(std::is_same_v<
                  ddge::util::meta::mimic_t<T, const volatile float&>,
                  const volatile Naked&>);
    static_assert(std::is_same_v<
                  ddge::util::meta::mimic_t<T, const volatile float&&>,
                  const volatile Naked&&>);

    return true;
}

static_assert(check_type<int>());
static_assert(check_type<int&>());
static_assert(check_type<int&&>());
static_assert(check_type<const int>());
static_assert(check_type<const int&>());
static_assert(check_type<const int&&>());
static_assert(check_type<volatile int>());
static_assert(check_type<volatile int&>());
static_assert(check_type<volatile int&&>());
static_assert(check_type<const volatile int>());
static_assert(check_type<const volatile int&>());
static_assert(check_type<const volatile int&&>());

}   // namespace
